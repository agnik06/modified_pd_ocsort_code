#ifndef ASSOCIATION_HPP
#define ASSOCIATION_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <functional>

namespace ocsort {

// Association functions
using AssociationFunc = std::function<Eigen::MatrixXf(const Eigen::MatrixXf&, const Eigen::MatrixXf&)>;

// Compute IoU between two sets of bounding boxes
Eigen::MatrixXf iou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2);

// Compute GIoU between two sets of bounding boxes
Eigen::MatrixXf giou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2);

// Compute DIoU between two sets of bounding boxes
Eigen::MatrixXf diou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2);

// Compute CIoU between two sets of bounding boxes
Eigen::MatrixXf ciou_batch(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2);

// Compute center distance between two sets of bounding boxes
Eigen::MatrixXf ct_dist(const Eigen::MatrixXf& bboxes1, const Eigen::MatrixXf& bboxes2);

// Speed direction batch calculation
// Returns a (2*n_dets x n_tracks) matrix where first half is Y direction, second half is X direction
Eigen::MatrixXf speed_direction_batch(
    const Eigen::MatrixXf& dets, const Eigen::MatrixXf& tracks);

// Linear assignment using Hungarian algorithm
std::vector<std::vector<int>> linear_assignment(const Eigen::MatrixXf& cost_matrix);

// Associate detections to trackers (basic version)
std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate_detections_to_trackers(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    float iou_threshold = 0.3f);

// Associate with velocity direction consistency
std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    float iou_threshold,
    const Eigen::MatrixXf& velocities,
    const Eigen::MatrixXf& previous_obs,
    float vdc_weight);

// Associate with categories (for KITTI)
std::tuple<std::vector<std::vector<int>>, std::vector<int>, std::vector<int>>
associate_kitti(
    const Eigen::MatrixXf& detections,
    const Eigen::MatrixXf& trackers,
    const Eigen::VectorXi& det_cates,
    float iou_threshold,
    const Eigen::MatrixXf& velocities,
    const Eigen::MatrixXf& previous_obs,
    float vdc_weight);

} // namespace ocsort

#endif // ASSOCIATION_HPP
