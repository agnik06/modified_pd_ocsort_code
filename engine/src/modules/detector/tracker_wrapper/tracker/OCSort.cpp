#include "OCSort.hpp"
#include "Utilities.hpp"
#include "Association.hpp"
#include <algorithm>
#include <unordered_map>

namespace ocsort {

OCSort::OCSort(float det_thresh_, int max_age_, int min_hits_, 
                float iou_threshold_, int delta_t_, std::string asso_func_, 
                float inertia_, bool use_byte_)
    : max_age(max_age_), min_hits(min_hits_), iou_threshold(iou_threshold_),
      det_thresh(det_thresh_), delta_t(delta_t_), inertia(inertia_), 
      use_byte(use_byte_) {
    
    trackers.clear();
    frame_count = 0;
    
    // Set up association functions
    std::unordered_map<std::string, AssociationFunc> ASSO_FUNCS = {
        {"iou", iou_batch},
        {"giou", giou_batch},
        {"diou", diou_batch},
        {"ciou", ciou_batch},
        {"ct_dist", ct_dist}
    };
    
    asso_func = ASSO_FUNCS[asso_func_];
    KalmanBoxTracker::count = 0;
}

std::vector<Eigen::RowVectorXf> OCSort::update(const Eigen::MatrixXf& output_results,
                                                 const Eigen::Vector2f& img_info,
                                                 const Eigen::Vector2f& img_size) {
    if (output_results.rows() == 0) {
        return std::vector<Eigen::RowVectorXf>();
    }
    
    frame_count++;
    
    // Post-process detections
    Eigen::MatrixXf bboxes = output_results.leftCols(4);
    Eigen::VectorXf scores = output_results.col(4);
    
    float img_h = img_info(0);
    float img_w = img_info(1);
    float scale = std::min(img_size(0) / img_h, img_size(1) / img_w);
    
    bboxes /= scale;
    
    Eigen::MatrixXf dets(output_results.rows(), 5);
    dets.leftCols(4) = bboxes;
    dets.col(4) = scores;
    
    // Split detections into high and low confidence
    Eigen::MatrixXf dets_second;
    Eigen::MatrixXf dets_first;
    
    for (int i = 0; i < dets.rows(); ++i) {
        if (scores(i) > 0.1f && scores(i) < det_thresh) {
            int new_row = dets_second.rows();
            dets_second.conservativeResize(new_row + 1, 5);
            dets_second.row(new_row) = dets.row(i);
        } else if (scores(i) >= det_thresh) {
            int new_row = dets_first.rows();
            dets_first.conservativeResize(new_row + 1, 5);
            dets_first.row(new_row) = dets.row(i);
        }
    }
    
    // Collect predictions and identify invalid trackers
    std::vector<int> to_del;
    std::vector<Eigen::VectorXf> predictions;
    
    for (size_t i = 0; i < trackers.size(); ++i) {
        Eigen::VectorXf pos = trackers[i].predict();
        predictions.push_back(pos);
        
        if (pos.array().isNaN().any()) {
            to_del.push_back(i);
        }
    }
    
    // Remove invalid trackers
    for (auto it = to_del.rbegin(); it != to_del.rend(); ++it) {
        trackers.erase(trackers.begin() + *it);
    }
    
    // Get predicted locations from existing trackers
    Eigen::MatrixXf trks = Eigen::MatrixXf::Zero(trackers.size(), 5);
    for (size_t i = 0; i < trackers.size(); ++i) {
        Eigen::Vector4f state = trackers[i].get_state();
        trks(i, 0) = state(0);
        trks(i, 1) = state(1);
        trks(i, 2) = state(2);
        trks(i, 3) = state(3);
        trks(i, 4) = 0.0f;
    }
    
    // Collect velocities and observations
    Eigen::MatrixXf velocities(trackers.size(), 2);
    Eigen::MatrixXf last_boxes(trackers.size(), 5);
    Eigen::MatrixXf k_observations(trackers.size(), 5);
    
    for (size_t i = 0; i < trackers.size(); ++i) {
        velocities(i, 0) = trackers[i].velocity(0);
        velocities(i, 1) = trackers[i].velocity(1);
        for (int j = 0; j < 5; ++j) {
            last_boxes(i, j) = trackers[i].last_observation(j);
        }
        Eigen::VectorXf k_obs = k_previous_obs(trackers[i].observations, 
                                               trackers[i].age, delta_t);
        for (int j = 0; j < 5; ++j) {
            k_observations(i, j) = k_obs(j);
        }
    }
    
    // First round of association
    std::vector<std::vector<int>> matched;
    std::vector<int> unmatched_dets;
    std::vector<int> unmatched_trks;
    
    std::tie(matched, unmatched_dets, unmatched_trks) = associate(
        dets_first, trks, iou_threshold, velocities, k_observations, inertia);
    
    for (const auto& m : matched) {
        Eigen::VectorXf tmp_bbox = dets_first.row(m[0]).transpose();
        trackers[m[1]].update(&tmp_bbox);
    }
    
    // Second round of association by OCR (BYTE)
    if (use_byte && dets_second.rows() > 0 && !unmatched_trks.empty()) {
        Eigen::MatrixXf u_trks(unmatched_trks.size(), trks.cols());
        for (size_t i = 0; i < unmatched_trks.size(); ++i) {
            u_trks.row(i) = trks.row(unmatched_trks[i]);
        }
        
        Eigen::MatrixXf iou_left = asso_func(dets_second, u_trks);
        
        if (iou_left.maxCoeff() > iou_threshold) {
            Eigen::MatrixXf cost_matrix = -iou_left;
            std::vector<std::vector<int>> matched_indices = linear_assignment(cost_matrix);
            
            std::vector<int> to_remove_trk_indices;
            for (const auto& m : matched_indices) {
                int det_ind = m[0];
                int trk_ind = unmatched_trks[m[1]];
                
                if (iou_left(m[0], m[1]) < iou_threshold) continue;
                
                Eigen::VectorXf tmp_bbox = dets_second.row(det_ind).transpose();
                trackers[trk_ind].update(&tmp_bbox);
                to_remove_trk_indices.push_back(trk_ind);
            }
            
            std::sort(to_remove_trk_indices.begin(), to_remove_trk_indices.end());
            std::sort(unmatched_trks.begin(), unmatched_trks.end());
            
            std::vector<int> tmp_res(unmatched_trks.size());
            auto end_it = std::set_difference(unmatched_trks.begin(), unmatched_trks.end(),
                                              to_remove_trk_indices.begin(), to_remove_trk_indices.end(),
                                              tmp_res.begin());
            tmp_res.resize(end_it - tmp_res.begin());
            unmatched_trks = tmp_res;
        }
    }
    
    // Re-association for remaining unmatched
    if (!unmatched_dets.empty() && !unmatched_trks.empty()) {
        Eigen::MatrixXf left_dets(unmatched_dets.size(), 5);
        for (size_t i = 0; i < unmatched_dets.size(); ++i) {
            left_dets.row(i) = dets_first.row(unmatched_dets[i]);
        }
        
        Eigen::MatrixXf left_trks(unmatched_trks.size(), last_boxes.cols());
        for (size_t i = 0; i < unmatched_trks.size(); ++i) {
            left_trks.row(i) = last_boxes.row(unmatched_trks[i]);
        }
        
        Eigen::MatrixXf iou_left = asso_func(left_dets, left_trks);
        
        if (iou_left.maxCoeff() > iou_threshold) {
            Eigen::MatrixXf cost_matrix = -iou_left;
            std::vector<std::vector<int>> rematched_indices = linear_assignment(cost_matrix);
            
            std::vector<int> to_remove_det_indices;
            std::vector<int> to_remove_trk_indices;
            
            for (const auto& m : rematched_indices) {
                int det_ind = unmatched_dets[m[0]];
                int trk_ind = unmatched_trks[m[1]];
                
                if (iou_left(m[0], m[1]) < iou_threshold) continue;
                
                Eigen::VectorXf tmp_bbox = dets_first.row(det_ind).transpose();
                trackers[trk_ind].update(&tmp_bbox);
                to_remove_det_indices.push_back(det_ind);
                to_remove_trk_indices.push_back(trk_ind);
            }
            
            std::sort(to_remove_det_indices.begin(), to_remove_det_indices.end());
            std::sort(unmatched_dets.begin(), unmatched_dets.end());
            
            std::vector<int> tmp_res(unmatched_dets.size());
            auto end_it = std::set_difference(unmatched_dets.begin(), unmatched_dets.end(),
                                              to_remove_det_indices.begin(), to_remove_det_indices.end(),
                                              tmp_res.begin());
            tmp_res.resize(end_it - tmp_res.begin());
            unmatched_dets = tmp_res;
            
            std::sort(to_remove_trk_indices.begin(), to_remove_trk_indices.end());
            std::sort(unmatched_trks.begin(), unmatched_trks.end());
            
            tmp_res.clear();
            tmp_res.resize(unmatched_trks.size());
            end_it = std::set_difference(unmatched_trks.begin(), unmatched_trks.end(),
                                        to_remove_trk_indices.begin(), to_remove_trk_indices.end(),
                                        tmp_res.begin());
            tmp_res.resize(end_it - tmp_res.begin());
            unmatched_trks = tmp_res;
        }
    }
    
    // Update unmatched trackers
    for (int m : unmatched_trks) {
        trackers[m].update(nullptr);
    }
    
    // Create new trackers for unmatched detections
    for (int i : unmatched_dets) {
        Eigen::VectorXf tmp_bbox = dets_first.row(i).transpose();
        KalmanBoxTracker trk(tmp_bbox, delta_t);
        trackers.push_back(trk);
    }
    
    // Collect results
    std::vector<Eigen::RowVectorXf> ret;
    for (int i = trackers.size() - 1; i >= 0; --i) {
        Eigen::Vector4f d;
        
        if (trackers[i].last_observation.sum() < 0) {
            d = trackers[i].get_state();
        } else {
            d = trackers[i].last_observation.head(4);
        }
        
        if (trackers[i].time_since_update < 1 &&
        (trackers[i].hit_streak>=min_hits || frame_count<=min_hits)) {
            Eigen::RowVectorXf tracking_res(5);
            tracking_res << d(0), d(1), d(2), d(3), trackers[i].id + 1;
            ret.push_back(tracking_res);
        }
        
        // Remove dead tracklet
        if (trackers[i].time_since_update > max_age) {
            trackers.erase(trackers.begin() + i);
        }
    }
    
    return ret;
}

std::vector<Eigen::RowVectorXf> OCSort::update_public(const Eigen::MatrixXf& dets,
                                                        const Eigen::VectorXi& cates,
                                                        const Eigen::VectorXf& scores) {
    frame_count++;
    
    // Filter detections by threshold
    std::vector<int> remain_inds_vec;
    for (int i = 0; i < scores.rows(); ++i) {
        if (scores(i) > det_thresh) {
            remain_inds_vec.push_back(i);
        }
    }
    
    Eigen::MatrixXf filtered_dets(remain_inds_vec.size(), dets.cols());
    Eigen::VectorXi filtered_cates(remain_inds_vec.size());
    
    for (size_t i = 0; i < remain_inds_vec.size(); ++i) {
        int idx = remain_inds_vec[i];
        filtered_dets.row(i) = dets.row(idx);
        filtered_cates(i) = cates(idx);
    }
    
    // Collect predictions and identify invalid trackers
    std::vector<int> to_del;
    
    for (size_t i = 0; i < trackers.size(); ++i) {
        Eigen::VectorXf pos = trackers[i].predict();
        
        if (pos.array().isNaN().any()) {
            to_del.push_back(i);
        }
    }
    
    // Remove invalid trackers
    for (auto it = to_del.rbegin(); it != to_del.rend(); ++it) {
        trackers.erase(trackers.begin() + *it);
    }
    
    // Get predicted locations from existing trackers
    Eigen::MatrixXf trks = Eigen::MatrixXf::Zero(trackers.size(), 5);
    for (size_t i = 0; i < trackers.size(); ++i) {
        Eigen::Vector4f state = trackers[i].get_state();
        trks(i, 0) = state(0);
        trks(i, 1) = state(1);
        trks(i, 2) = state(2);
        trks(i, 3) = state(3);
        trks(i, 4) = 0.0f;
    }
    
    // Collect velocities and observations
    Eigen::MatrixXf velocities(trackers.size(), 2);
    Eigen::MatrixXf last_boxes(trackers.size(), 5);
    Eigen::MatrixXf k_observations(trackers.size(), 5);
    
    for (size_t i = 0; i < trackers.size(); ++i) {
        velocities(i, 0) = trackers[i].velocity(0);
        velocities(i, 1) = trackers[i].velocity(1);
        for (int j = 0; j < 5; ++j) {
            last_boxes(i, j) = trackers[i].last_observation(j);
        }
        Eigen::VectorXf k_obs = k_previous_obs(trackers[i].observations, 
                                               trackers[i].age, delta_t);
        for (int j = 0; j < 5; ++j) {
            k_observations(i, j) = k_obs(j);
        }
    }
    
    // First round of association
    std::vector<std::vector<int>> matched;
    std::vector<int> unmatched_dets;
    std::vector<int> unmatched_trks;
    
    std::tie(matched, unmatched_dets, unmatched_trks) = associate_kitti(
        filtered_dets, trks, filtered_cates, iou_threshold, 
        velocities, k_observations, inertia);
    
    for (const auto& m : matched) {
        Eigen::VectorXf tmp_bbox = filtered_dets.row(m[0]).transpose();
        trackers[m[1]].update(&tmp_bbox);
    }
    
    // Re-association for remaining unmatched
    if (!unmatched_dets.empty() && !unmatched_trks.empty()) {
        Eigen::MatrixXf left_dets(unmatched_dets.size(), filtered_dets.cols());
        for (size_t i = 0; i < unmatched_dets.size(); ++i) {
            left_dets.row(i) = filtered_dets.row(unmatched_dets[i]);
        }
        
        Eigen::MatrixXf left_trks(unmatched_trks.size(), last_boxes.cols());
        for (size_t i = 0; i < unmatched_trks.size(); ++i) {
            left_trks.row(i) = last_boxes.row(unmatched_trks[i]);
        }
        
        Eigen::MatrixXf iou_left = asso_func(left_dets, left_trks);
        
        // Category mismatch cost
        Eigen::MatrixXf cate_matrix = Eigen::MatrixXf::Zero(unmatched_dets.size(), unmatched_trks.size());
        for (size_t i = 0; i < unmatched_dets.size(); ++i) {
            for (size_t j = 0; j < unmatched_trks.size(); ++j) {
                if (filtered_cates(unmatched_dets[i]) != trks(unmatched_trks[j], 4)) {
                    cate_matrix(i, j) = -1e6f;
                }
            }
        }
        
        Eigen::MatrixXf cost_matrix = -iou_left - cate_matrix;
        
        if (iou_left.maxCoeff() > iou_threshold - 0.1f) {
            std::vector<std::vector<int>> rematched_indices = linear_assignment(cost_matrix);
            
            std::vector<int> to_remove_det_indices;
            std::vector<int> to_remove_trk_indices;
            
            for (const auto& m : rematched_indices) {
                int det_ind = unmatched_dets[m[0]];
                int trk_ind = unmatched_trks[m[1]];
                
                if (iou_left(m[0], m[1]) < iou_threshold - 0.1f) continue;
                
                Eigen::VectorXf tmp_bbox = filtered_dets.row(det_ind).transpose();
                trackers[trk_ind].update(&tmp_bbox);
                to_remove_det_indices.push_back(det_ind);
                to_remove_trk_indices.push_back(trk_ind);
            }
            
            std::sort(to_remove_det_indices.begin(), to_remove_det_indices.end());
            std::sort(unmatched_dets.begin(), unmatched_dets.end());
            
            std::vector<int> tmp_res(unmatched_dets.size());
            auto end_it = std::set_difference(unmatched_dets.begin(), unmatched_dets.end(),
                                              to_remove_det_indices.begin(), to_remove_det_indices.end(),
                                              tmp_res.begin());
            tmp_res.resize(end_it - tmp_res.begin());
            unmatched_dets = tmp_res;
            
            std::sort(to_remove_trk_indices.begin(), to_remove_trk_indices.end());
            std::sort(unmatched_trks.begin(), unmatched_trks.end());
            
            tmp_res.clear();
            tmp_res.resize(unmatched_trks.size());
            end_it = std::set_difference(unmatched_trks.begin(), unmatched_trks.end(),
                                        to_remove_trk_indices.begin(), to_remove_trk_indices.end(),
                                        tmp_res.begin());
            tmp_res.resize(end_it - tmp_res.begin());
            unmatched_trks = tmp_res;
        }
    }
    
    // Create new trackers for unmatched detections
    for (int i : unmatched_dets) {
        Eigen::VectorXf tmp_bbox = filtered_dets.row(i).transpose();
        KalmanBoxTracker trk(tmp_bbox, delta_t);
        trackers.push_back(trk);
    }
    
    // Collect results
    std::vector<Eigen::RowVectorXf> ret;
    for (int i = trackers.size() - 1; i >= 0; --i) {
        Eigen::Vector4f d;
        
        if (trackers[i].last_observation.sum() > 0) {
            d = trackers[i].last_observation.head(4);
        } else {
            d = trackers[i].get_state();
        }
        
        if (trackers[i].time_since_update < 1) {
            if (frame_count <= min_hits || trackers[i].hit_streak >= min_hits) {
                Eigen::RowVectorXf tracking_res(7);
                tracking_res << d(0), d(1), d(2), d(3), trackers[i].id + 1, 
                                 0.0f, 0.0f;  // Category and time offset
                ret.push_back(tracking_res);
            }
        }
        
        // Remove dead tracklet
        if (trackers[i].time_since_update > max_age) {
            trackers.erase(trackers.begin() + i);
        }
    }
    
    return ret;
}

} // namespace ocsort
