#include "Association.hpp"
#include "Utilities.hpp"
#include "lapjv.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>

namespace ocsort {

Eigen::MatrixXf iou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2) {
    int n1 = bboxes1.rows();
    int n2 = bboxes2.rows();
    
    Eigen::MatrixXf iou_matrix(n1, n2);
    
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            // Intersection
            float xx1 = std::max(bboxes1(i, 0), bboxes2(j, 0));
            float yy1 = std::max(bboxes1(i, 1), bboxes2(j, 1));
            float xx2 = std::min(bboxes1(i, 2), bboxes2(j, 2));
            float yy2 = std::min(bboxes1(i, 3), bboxes2(j, 3));
            
            float w = std::max(0.0f, xx2 - xx1);
            float h = std::max(0.0f, yy2 - yy1);
            float wh = w * h;
            
            // Union
            float area1 = (bboxes1(i, 2) - bboxes1(i, 0)) * (bboxes1(i, 3) - bboxes1(i, 1));
            float area2 = (bboxes2(j, 2) - bboxes2(j, 0)) * (bboxes2(j, 3) - bboxes2(j, 1));
            float union_area = area1 + area2 - wh;
            
            iou_matrix(i, j) = wh / (union_area + 1e-6f);
        }
    }
    
    return iou_matrix;
}

Eigen::MatrixXf giou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2) {
    int n1 = bboxes1.rows();
    int n2 = bboxes2.rows();
    
    Eigen::MatrixXf giou_matrix(n1, n2);
    
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            // Intersection
            float xx1 = std::max(bboxes1(i, 0), bboxes2(j, 0));
            float yy1 = std::max(bboxes1(i, 1), bboxes2(j, 1));
            float xx2 = std::min(bboxes1(i, 2), bboxes2(j, 2));
            float yy2 = std::min(bboxes1(i, 3), bboxes2(j, 3));
            
            float w = std::max(0.0f, xx2 - xx1);
            float h = std::max(0.0f, yy2 - yy1);
            float wh = w * h;
            
            // Union
            float area1 = (bboxes1(i, 2) - bboxes1(i, 0)) * (bboxes1(i, 3) - bboxes1(i, 1));
            float area2 = (bboxes2(j, 2) - bboxes2(j, 0)) * (bboxes2(j, 3) - bboxes2(j, 1));
            float union_area = area1 + area2 - wh;
            
            float iou = wh / (union_area + 1e-6f);
            
            // Enclosing box
            float xxc1 = std::min(bboxes1(i, 0), bboxes2(j, 0));
            float yyc1 = std::min(bboxes1(i, 1), bboxes2(j, 1));
            float xxc2 = std::max(bboxes1(i, 2), bboxes2(j, 2));
            float yyc2 = std::max(bboxes1(i, 3), bboxes2(j, 3));
            
            float wc = xxc2 - xxc1;
            float hc = yyc2 - yyc1;
            float area_enclose = wc * hc;
            
            float giou = iou - (area_enclose - union_area) / (area_enclose + 1e-6f);
            giou_matrix(i, j) = (giou + 1.0f) / 2.0f;  // resize from (-1,1) to (0,1)
        }
    }
    
    return giou_matrix;
}

Eigen::MatrixXf diou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2) {
    int n1 = bboxes1.rows();
    int n2 = bboxes2.rows();
    
    Eigen::MatrixXf diou_matrix(n1, n2);
    
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            // Intersection
            float xx1 = std::max(bboxes1(i, 0), bboxes2(j, 0));
            float yy1 = std::max(bboxes1(i, 1), bboxes2(j, 1));
            float xx2 = std::min(bboxes1(i, 2), bboxes2(j, 2));
            float yy2 = std::min(bboxes1(i, 3), bboxes2(j, 3));
            
            float w = std::max(0.0f, xx2 - xx1);
            float h = std::max(0.0f, yy2 - yy1);
            float wh = w * h;
            
            // Union
            float area1 = (bboxes1(i, 2) - bboxes1(i, 0)) * (bboxes1(i, 3) - bboxes1(i, 1));
            float area2 = (bboxes2(j, 2) - bboxes2(j, 0)) * (bboxes2(j, 3) - bboxes2(j, 1));
            float union_area = area1 + area2 - wh;
            
            float iou = wh / (union_area + 1e-6f);
            
            // Center points
            float centerx1 = (bboxes1(i, 0) + bboxes1(i, 2)) / 2.0f;
            float centery1 = (bboxes1(i, 1) + bboxes1(i, 3)) / 2.0f;
            float centerx2 = (bboxes2(j, 0) + bboxes2(j, 2)) / 2.0f;
            float centery2 = (bboxes2(j, 1) + bboxes2(j, 3)) / 2.0f;
            
            float inner_diag = (centerx1 - centerx2) * (centerx1 - centerx2) + 
                              (centery1 - centery2) * (centery1 - centery2);
            
            // Enclosing box
            float xxc1 = std::min(bboxes1(i, 0), bboxes2(j, 0));
            float yyc1 = std::min(bboxes1(i, 1), bboxes2(j, 1));
            float xxc2 = std::max(bboxes1(i, 2), bboxes2(j, 2));
            float yyc2 = std::max(bboxes1(i, 3), bboxes2(j, 3));
            
            float outer_diag = (xxc2 - xxc1) * (xxc2 - xxc1) + 
                              (yyc2 - yyc1) * (yyc2 - yyc1);
            
            float diou = iou - inner_diag / (outer_diag + 1e-6f);
            diou_matrix(i, j) = (diou + 1.0f) / 2.0f;  // resize from (-1,1) to (0,1)
        }
    }
    
    return diou_matrix;
}

Eigen::MatrixXf ciou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2) {
    int n1 = bboxes1.rows();
    int n2 = bboxes2.rows();
    
    Eigen::MatrixXf ciou_matrix(n1, n2);
    const float pi = 3.14159265358979323846f;
    
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            // Intersection
            float xx1 = std::max(bboxes1(i, 0), bboxes2(j, 0));
            float yy1 = std::max(bboxes1(i, 1), bboxes2(j, 1));
            float xx2 = std::min(bboxes1(i, 2), bboxes2(j, 2));
            float yy2 = std::min(bboxes1(i, 3), bboxes2(j, 3));
            
            float w = std::max(0.0f, xx2 - xx1);
            float h = std::max(0.0f, yy2 - yy1);
            float wh = w * h;
            
            // Union
            float area1 = (bboxes1(i, 2) - bboxes1(i, 0)) * (bboxes1(i, 3) - bboxes1(i, 1));
            float area2 = (bboxes2(j, 2) - bboxes2(j, 0)) * (bboxes2(j, 3) - bboxes2(j, 1));
            float union_area = area1 + area2 - wh;
            
            float iou = wh / (union_area + 1e-6f);
            
            // Center points
            float centerx1 = (bboxes1(i, 0) + bboxes1(i, 2)) / 2.0f;
            float centery1 = (bboxes1(i, 1) + bboxes1(i, 3)) / 2.0f;
            float centerx2 = (bboxes2(j, 0) + bboxes2(j, 2)) / 2.0f;
            float centery2 = (bboxes2(j, 1) + bboxes2(j, 3)) / 2.0f;
            
            float inner_diag = (centerx1 - centerx2) * (centerx1 - centerx2) + 
                              (centery1 - centery2) * (centery1 - centery2);
            
            // Enclosing box
            float xxc1 = std::min(bboxes1(i, 0), bboxes2(j, 0));
            float yyc1 = std::min(bboxes1(i, 1), bboxes2(j, 1));
            float xxc2 = std::max(bboxes1(i, 2), bboxes2(j, 2));
            float yyc2 = std::max(bboxes1(i, 3), bboxes2(j, 3));
            
            float outer_diag = (xxc2 - xxc1) * (xxc2 - xxc1) + 
                              (yyc2 - yyc1) * (yyc2 - yyc1);
            
            // Aspect ratio consistency
            float w1 = bboxes1(i, 2) - bboxes1(i, 0);
            float h1 = bboxes1(i, 3) - bboxes1(i, 1);
            float w2 = bboxes2(j, 2) - bboxes2(j, 0);
            float h2 = bboxes2(j, 3) - bboxes2(j, 1);
            
            h2 = h2 + 1.0f;
            h1 = h1 + 1.0f;
            
            float arctan = std::atan(w2 / h2) - std::atan(w1 / h1);
            float v = (4.0f / (pi * pi)) * arctan * arctan;
            float S = 1.0f - iou;
            float alpha = v / (S + v);
            
            float ciou = iou - inner_diag / (outer_diag + 1e-6f) - alpha * v;
            ciou_matrix(i, j) = (ciou + 1.0f) / 2.0f;  // resize from (-1,1) to (0,1)
        }
    }
    
    return ciou_matrix;
}

Eigen::MatrixXf ct_dist(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2) {
    int n1 = bboxes1.rows();
    int n2 = bboxes2.rows();
    
    Eigen::MatrixXf dist_matrix(n1, n2);
    
    for (int i = 0; i < n1; ++i) {
        for (int j = 0; j < n2; ++j) {
            float cx1 = (bboxes1(i, 0) + bboxes1(i, 2)) / 2.0f;
            float cy1 = (bboxes1(i, 1) + bboxes1(i, 3)) / 2.0f;
            float cx2 = (bboxes2(j, 0) + bboxes2(j, 2)) / 2.0f;
            float cy2 = (bboxes2(j, 1) + bboxes2(j, 3)) / 2.0f;
            
            float ct_dist2 = (cx1 - cx2) * (cx1 - cx2) + (cy1 - cy2) * (cy1 - cy2);
            dist_matrix(i, j) = std::sqrt(ct_dist2);
        }
    }
    
    // Normalize to (0, 1)
    float max_dist = dist_matrix.maxCoeff();
    if (max_dist > 1e-6f) {
        dist_matrix = dist_matrix / max_dist;
        dist_matrix = Eigen::MatrixXf::Ones(dist_matrix.rows(), dist_matrix.cols()) * max_dist - dist_matrix;
    }
    return dist_matrix;
}

Eigen::MatrixXf speed_direction_batch(
    const Eigen::MatrixXf& dets, const Eigen::MatrixXf& tracks) {
    
    int n_dets = dets.rows();
    int n_tracks = tracks.rows();
    
    // Calculate center points for detections - shape (n_dets, 1)
    Eigen::MatrixXf CX1(n_dets, 1);
    Eigen::MatrixXf CY1(n_dets, 1);
    for (int i = 0; i < n_dets; ++i) {
        CX1(i, 0) = (dets(i, 0) + dets(i, 2)) / 2.0f;
        CY1(i, 0) = (dets(i, 1) + dets(i, 3)) / 2.0f;
    }
    
    // Calculate center points for tracks - shape (n_tracks, 1)
    Eigen::MatrixXf CX2(n_tracks, 1);
    Eigen::MatrixXf CY2(n_tracks, 1);
    for (int i = 0; i < n_tracks; ++i) {
        // Only use first 4 columns for bbox coordinates
        CX2(i, 0) = (tracks(i, 0) + tracks(i, 2)) / 2.0f;
        CY2(i, 0) = (tracks(i, 1) + tracks(i, 3)) / 2.0f;
    }
    
    // Broadcast to create (n_tracks x n_dets) matrices - matching Python's output
    // (n_tracks, 1) - (1, n_dets) after transpose
    Eigen::MatrixXf result_Y = CY2 * Eigen::MatrixXf::Ones(1, n_dets) - 
                               Eigen::MatrixXf::Ones(n_tracks, 1) * CY1.transpose();
    Eigen::MatrixXf result_X = CX2 * Eigen::MatrixXf::Ones(1, n_dets) - 
                               Eigen::MatrixXf::Ones(n_tracks, 1) * CX1.transpose();
    
    // Stack vertically to return a single matrix
    Eigen::MatrixXf result(2 * n_tracks, n_dets);
    result.topRows(n_tracks) = result_Y;
    result.bottomRows(n_tracks) = result_X;
    
    return result;
}

std::vector<std::vector<int>> linear_assignment(const Eigen::MatrixXf& cost_matrix) {
    int n = cost_matrix.rows();
    int m = cost_matrix.cols();
    
    std::vector<std::vector<float>> cost_vec(n, std::vector<float>(m));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            cost_vec[i][j] = cost_matrix(i, j);
        }
    }
    
    std::vector<int> rowsol, colsol;
    execLapjv(cost_vec, rowsol, colsol, true, 0.01f, true);
    
    std::vector<std::vector<int>> matched_indices;
    for (int i = 0; i < rowsol.size(); ++i) {
        if (rowsol[i] >= 0) {
            matched_indices.push_back({i, rowsol[i]});
        }
    }
    
    return matched_indices;
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate_detections_to_trackers(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    float iou_threshold) {
    
    if (trackers.rows() == 0) {
        std::vector<std::vector<int>> matched_indices;
        std::vector<int> unmatched_detections(detections.rows());
        std::vector<int> unmatched_trackers;
        for (int i = 0; i < detections.rows(); ++i) {
            unmatched_detections[i] = i;
        }
        return std::make_tuple(matched_indices, unmatched_detections, unmatched_trackers);
    }
    
    Eigen::MatrixXf iou_matrix = iou_batch(detections, trackers);
    
    std::vector<std::vector<int>> matched_indices;
    // Check if matrix is not empty before calling minCoeff()
    if (iou_matrix.rows() > 0 && iou_matrix.cols() > 0 && iou_matrix.minCoeff() > 0) {
        Eigen::MatrixXi a = (iou_matrix.array() > iou_threshold).cast<int>();
        
        // Check if simple assignment is possible
        bool simple_assignment = true;
        for (int i = 0; i < a.rows() && simple_assignment; ++i) {
            if (a.row(i).sum() > 1) simple_assignment = false;
        }
        for (int j = 0; j < a.cols() && simple_assignment; ++j) {
            if (a.col(j).sum() > 1) simple_assignment = false;
        }
        
        if (simple_assignment) {
            for (int i = 0; i < a.rows(); ++i) {
                for (int j = 0; j < a.cols(); ++j) {
                    if (a(i, j)) {
                        matched_indices.push_back({i, j});
                    }
                }
            }
        } else {
            matched_indices = linear_assignment(-iou_matrix);
        }
    }
    
    std::vector<int> unmatched_detections;
    std::vector<int> unmatched_trackers;
    std::vector<bool> det_matched(detections.rows(), false);
    std::vector<bool> trk_matched(trackers.rows(), false);
    
    for (const auto& m : matched_indices) {
        det_matched[m[0]] = true;
        trk_matched[m[1]] = true;
    }
    
    for (int i = 0; i < detections.rows(); ++i) {
        if (!det_matched[i]) {
            unmatched_detections.push_back(i);
        }
    }
    
    for (int i = 0; i < trackers.rows(); ++i) {
        if (!trk_matched[i]) {
            unmatched_trackers.push_back(i);
        }
    }
    
    // Filter out matched with low IOU
    std::vector<std::vector<int>> final_matches;
    for (const auto& m : matched_indices) {
        if (iou_matrix(m[0], m[1]) < iou_threshold) {
            unmatched_detections.push_back(m[0]);
            unmatched_trackers.push_back(m[1]);
        } else {
            final_matches.push_back(m);
        }
    }
    
    return std::make_tuple(final_matches, unmatched_detections, unmatched_trackers);
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    float iou_threshold,
    const Eigen::MatrixXf& velocities,
    const Eigen::MatrixXf& previous_obs,
    float vdc_weight) {
    
    int n_dets = detections.rows();
    int n_tracks = trackers.rows();
    
    if (trackers.rows() == 0) {
        std::vector<std::vector<int>> matched_indices;
        std::vector<int> unmatched_detections(n_dets);
        std::vector<int> unmatched_trackers;
        for (int i = 0; i < n_dets; ++i) {
            unmatched_detections[i] = i;
        }
        return std::make_tuple(matched_indices, unmatched_detections, unmatched_trackers);
    }
    
    // Handle edge cases early
    if (previous_obs.rows() == 0 || n_tracks == 0) {
        // Fall back to basic IOU association without velocity
        return associate_detections_to_trackers(detections, trackers, iou_threshold);
    }
    
    Eigen::MatrixXf speed_result = speed_direction_batch(detections, previous_obs);
    
    // speed_result is (2*n_tracks x n_dets) - first n_tracks rows are Y, next n_tracks are X
    if (speed_result.rows() != 2 * n_tracks || speed_result.cols() != n_dets) {
        // Handle dimension mismatch - fall back to basic association
        return associate_detections_to_trackers(detections, trackers, iou_threshold);
    }
    
    Eigen::MatrixXf Y = speed_result.topRows(n_tracks);
    Eigen::MatrixXf X = speed_result.bottomRows(n_tracks);
    
    // Verify X and Y dimensions - should be (n_tracks, n_dets)
    if (Y.rows() != n_tracks || Y.cols() != n_dets || X.rows() != n_tracks || X.cols() != n_dets) {
        return associate_detections_to_trackers(detections, trackers, iou_threshold);
    }
    
    // According to Python code: inertia_Y and inertia_X should be (n_tracks, n_dets)
    // velocities is (n_tracks, 2), so velocities(:,0) is (n_tracks,)
    // We need to broadcast this to (n_tracks, n_dets)
    Eigen::MatrixXf inertia_Y(n_tracks, n_dets);
    Eigen::MatrixXf inertia_X(n_tracks, n_dets);
    
    for (int j = 0; j < n_tracks; ++j) {
        if (velocities.rows() > j) {
            for (int i = 0; i < n_dets; ++i) {
                inertia_Y(j, i) = velocities(j, 0);
                inertia_X(j, i) = velocities(j, 1);
            }
        }
    }
    
    Eigen::MatrixXf diff_angle_cos = inertia_X.array() * X.array() + inertia_Y.array() * Y.array();
    diff_angle_cos = diff_angle_cos.array().max(-1.0f).min(1.0f);
    Eigen::MatrixXf diff_angle = diff_angle_cos.array().acos();
    diff_angle = (M_PI / 2.0f - diff_angle.array().abs()) / M_PI;
    
    Eigen::VectorXf valid_mask = Eigen::VectorXf::Ones(previous_obs.rows());
    for (int i = 0; i < previous_obs.rows(); ++i) {
        if (previous_obs(i, 4) < 0) {
            valid_mask(i) = 0.0f;
        }
    }
    
    Eigen::MatrixXf valid_mask_mat(n_tracks, n_dets);
    for (int i = 0; i < n_tracks; ++i) {
        for (int j = 0; j < n_dets; ++j) {
            valid_mask_mat(i, j) = valid_mask(i);
        }
    }
    
    Eigen::MatrixXf iou_matrix = iou_batch(detections, trackers);
    Eigen::MatrixXf scores(n_dets, n_tracks);
    for (int i = 0; i < n_dets; ++i) {
        for (int j = 0; j < n_tracks; ++j) {
            scores(i, j) = detections(i, 4);
        }
    }
    
    Eigen::MatrixXf angle_diff_cost = (valid_mask_mat.array() * diff_angle.array()) * vdc_weight;
    angle_diff_cost = angle_diff_cost.transpose().eval();
    angle_diff_cost = angle_diff_cost.array() * scores.array();
    
    std::vector<std::vector<int>> matched_indices;
    // Check if matrix is not empty before calling minCoeff()
    if (iou_matrix.rows() > 0 && iou_matrix.cols() > 0 && iou_matrix.minCoeff() > 0) {
        Eigen::MatrixXi a = (iou_matrix.array() > iou_threshold).cast<int>();
        
        bool simple_assignment = true;
        for (int i = 0; i < a.rows() && simple_assignment; ++i) {
            if (a.row(i).sum() > 1) simple_assignment = false;
        }
        for (int j = 0; j < a.cols() && simple_assignment; ++j) {
            if (a.col(j).sum() > 1) simple_assignment = false;
        }
        
        if (simple_assignment) {
            for (int i = 0; i < a.rows(); ++i) {
                for (int j = 0; j < a.cols(); ++j) {
                    if (a(i, j)) {
                        matched_indices.push_back({i, j});
                    }
                }
            }
        } else {
            Eigen::MatrixXf cost_matrix = -(iou_matrix + angle_diff_cost);
            matched_indices = linear_assignment(cost_matrix);
        }
    }
    
    std::vector<int> unmatched_detections;
    std::vector<int> unmatched_trackers;
    std::vector<bool> det_matched(detections.rows(), false);
    std::vector<bool> trk_matched(trackers.rows(), false);
    
    for (const auto& m : matched_indices) {
        det_matched[m[0]] = true;
        trk_matched[m[1]] = true;
    }
    
    for (int i = 0; i < detections.rows(); ++i) {
        if (!det_matched[i]) {
            unmatched_detections.push_back(i);
        }
    }
    
    for (int i = 0; i < trackers.rows(); ++i) {
        if (!trk_matched[i]) {
            unmatched_trackers.push_back(i);
        }
    }
    
    // Filter out matched with low IOU
    std::vector<std::vector<int>> final_matches;
    for (const auto& m : matched_indices) {
        if (iou_matrix(m[0], m[1]) < iou_threshold) {
            unmatched_detections.push_back(m[0]);
            unmatched_trackers.push_back(m[1]);
        } else {
            final_matches.push_back(m);
        }
    }
    
    return std::make_tuple(final_matches, unmatched_detections, unmatched_trackers);
}

std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate_kitti(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    const Eigen::VectorXi& det_cates,
    float iou_threshold,
    const Eigen::MatrixXf& velocities,
    const Eigen::MatrixXf& previous_obs,
    float vdc_weight) {
    
    if (trackers.rows() == 0) {
        std::vector<std::vector<int>> matched_indices;
        std::vector<int> unmatched_detections(detections.rows());
        std::vector<int> unmatched_trackers;
        for (int i = 0; i < detections.rows(); ++i) {
            unmatched_detections[i] = i;
        }
        return std::make_tuple(matched_indices, unmatched_detections, unmatched_trackers);
    }
    
    Eigen::MatrixXf kitti_speed_result = speed_direction_batch(detections, previous_obs);
    int n_dets = detections.rows();
    int n_tracks = trackers.rows();
    
    // speed_result is (2*n_tracks x n_dets) - first n_tracks rows are Y, next n_tracks are X
    if (kitti_speed_result.rows() != 2 * n_tracks || kitti_speed_result.cols() != n_dets) {
        // Handle empty or mismatched case
        std::vector<std::vector<int>> matched_indices;
        std::vector<int> unmatched_detections(n_dets);
        std::vector<int> unmatched_trackers(n_tracks);
        for (int i = 0; i < n_dets; ++i) unmatched_detections[i] = i;
        for (int i = 0; i < n_tracks; ++i) unmatched_trackers[i] = i;
        return std::make_tuple(matched_indices, unmatched_detections, unmatched_trackers);
    }
    
    Eigen::MatrixXf Y = kitti_speed_result.topRows(n_tracks);
    Eigen::MatrixXf X = kitti_speed_result.bottomRows(n_tracks);
    
    // Verify X and Y dimensions - should be (n_tracks, n_dets)
    if (Y.rows() != n_tracks || Y.cols() != n_dets || X.rows() != n_tracks || X.cols() != n_dets) {
        // Handle dimension mismatch
        std::vector<std::vector<int>> matched_indices;
        std::vector<int> unmatched_detections(n_dets);
        std::vector<int> unmatched_trackers(n_tracks);
        for (int i = 0; i < n_dets; ++i) unmatched_detections[i] = i;
        for (int i = 0; i < n_tracks; ++i) unmatched_trackers[i] = i;
        return std::make_tuple(matched_indices, unmatched_detections, unmatched_trackers);
    }
    
    // According to Python code: inertia_Y and inertia_X should be (n_tracks, n_dets)
    Eigen::MatrixXf inertia_Y(n_tracks, n_dets);
    Eigen::MatrixXf inertia_X(n_tracks, n_dets);
    
    for (int j = 0; j < n_tracks; ++j) {
        if (velocities.rows() > j) {
            for (int i = 0; i < n_dets; ++i) {
                inertia_Y(j, i) = velocities(j, 0);
                inertia_X(j, i) = velocities(j, 1);
            }
        }
    }
    
    Eigen::MatrixXf diff_angle_cos = inertia_X.array() * X.array() + inertia_Y.array() * Y.array();
    diff_angle_cos = diff_angle_cos.array().max(-1.0f).min(1.0f);
    Eigen::MatrixXf diff_angle = diff_angle_cos.array().acos();
    diff_angle = (M_PI / 2.0f - diff_angle.array().abs()) / M_PI;
    
    Eigen::VectorXf valid_mask = Eigen::VectorXf::Ones(previous_obs.rows());
    for (int i = 0; i < previous_obs.rows(); ++i) {
        if (previous_obs(i, 4) < 0) {
            valid_mask(i) = 0.0f;
        }
    }
    
    Eigen::MatrixXf valid_mask_mat(n_tracks, n_dets);
    for (int i = 0; i < n_tracks; ++i) {
        for (int j = 0; j < n_dets; ++j) {
            valid_mask_mat(i, j) = valid_mask(i);
        }
    }
    
    Eigen::MatrixXf scores(n_dets, n_tracks);
    for (int i = 0; i < n_dets; ++i) {
        for (int j = 0; j < n_tracks; ++j) {
            scores(i, j) = detections(i, 4);
        }
    }
    
    Eigen::MatrixXf angle_diff_cost = (valid_mask_mat.array() * diff_angle.array()) * vdc_weight;
    angle_diff_cost = angle_diff_cost.transpose().eval();
    angle_diff_cost = angle_diff_cost.array() * scores.array();
    
    // IoU cost
    Eigen::MatrixXf iou_matrix = iou_batch(detections, trackers);
    
    // Category mismatch cost
    Eigen::MatrixXf cate_matrix = Eigen::MatrixXf::Zero(n_dets, n_tracks);
    for (int i = 0; i < n_dets; ++i) {
        for (int j = 0; j < n_tracks; ++j) {
            if (det_cates(i) != trackers(j, 4)) {
                cate_matrix(i, j) = -1e6f;
            }
        }
    }
    
    Eigen::MatrixXf cost_matrix = -iou_matrix - angle_diff_cost - cate_matrix;
    
    std::vector<std::vector<int>> matched_indices;
    // Check if matrix is not empty before calling minCoeff()
    if (iou_matrix.rows() > 0 && iou_matrix.cols() > 0 && iou_matrix.minCoeff() > 0) {
        Eigen::MatrixXi a = (iou_matrix.array() > iou_threshold).cast<int>();
        
        bool simple_assignment = true;
        for (int i = 0; i < a.rows() && simple_assignment; ++i) {
            if (a.row(i).sum() > 1) simple_assignment = false;
        }
        for (int j = 0; j < a.cols() && simple_assignment; ++j) {
            if (a.col(j).sum() > 1) simple_assignment = false;
        }
        
        if (simple_assignment) {
            for (int i = 0; i < a.rows(); ++i) {
                for (int j = 0; j < a.cols(); ++j) {
                    if (a(i, j)) {
                        matched_indices.push_back({i, j});
                    }
                }
            }
        } else {
            matched_indices = linear_assignment(cost_matrix);
        }
    }
    
    std::vector<int> unmatched_detections;
    std::vector<int> unmatched_trackers;
    std::vector<bool> det_matched(detections.rows(), false);
    std::vector<bool> trk_matched(trackers.rows(), false);
    
    for (const auto& m : matched_indices) {
        det_matched[m[0]] = true;
        trk_matched[m[1]] = true;
    }
    
    for (int i = 0; i < detections.rows(); ++i) {
        if (!det_matched[i]) {
            unmatched_detections.push_back(i);
        }
    }
    
    for (int i = 0; i < trackers.rows(); ++i) {
        if (!trk_matched[i]) {
            unmatched_trackers.push_back(i);
        }
    }
    
    // Filter out matched with low IOU
    std::vector<std::vector<int>> final_matches;
    for (const auto& m : matched_indices) {
        if (iou_matrix(m[0], m[1]) < iou_threshold - 0.1f) {
            unmatched_detections.push_back(m[0]);
            unmatched_trackers.push_back(m[1]);
        } else {
            final_matches.push_back(m);
        }
    }
    
    return std::make_tuple(final_matches, unmatched_detections, unmatched_trackers);
}

} // namespace ocsort
