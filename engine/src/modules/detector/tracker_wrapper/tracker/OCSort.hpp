#ifndef OCSORT_HPP
#define OCSORT_HPP

#include "KalmanBoxTracker.hpp"
#include "Association.hpp"
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <functional>
#include <map>

namespace ocsort {

class OCSort {
public:
    OCSort(float det_thresh, int max_age = 30, int min_hits = 3,
            float iou_threshold = 0.3f, int delta_t = 3,
            std::string asso_func = "iou", float inertia = 0.2f, 
            bool use_byte = false);
    
    // Update with detections (standard version)
    std::vector<Eigen::RowVectorXf> update(const Eigen::MatrixXf& dets,
                                            const Eigen::Vector2f& img_info,
                                            const Eigen::Vector2f& img_size);
    
    // Update with detections (public version for KITTI)
    std::vector<Eigen::RowVectorXf> update_public(const Eigen::MatrixXf& dets,
                                                   const Eigen::VectorXi& cates,
                                                   const Eigen::VectorXf& scores);
    
    // Get current frame count
    inline int get_frame_count() const { return frame_count; }
    
private:
    int max_age;
    int min_hits;
    float iou_threshold;
    std::vector<KalmanBoxTracker> trackers;
    int frame_count;
    float det_thresh;
    int delta_t;
    AssociationFunc asso_func;
    float inertia;
    bool use_byte;
};

} // namespace ocsort

#endif // OCSORT_HPP
