#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <Eigen/Dense>
#include <vector>
#include <map>

namespace ocsort {

// Convert bounding box from [x1,y1,x2,y2] to [x,y,s,r] format
Eigen::VectorXf convert_bbox_to_z(const Eigen::VectorXf& bbox);

// Convert bounding box from [x,y,s,r] to [x1,y1,x2,y2] format
Eigen::VectorXf convert_x_to_bbox(const Eigen::VectorXf& x, float score = -1.0f);

// Calculate speed direction between two bounding boxes
Eigen::Vector2f speed_direction(const Eigen::RowVectorXf& bbox1, const Eigen::VectorXf& bbox2);

// Get k-th previous observation
Eigen::VectorXf k_previous_obs(const std::map<int, Eigen::RowVectorXf>& observations, int cur_age, int k);

} // namespace ocsort

#endif // UTILITIES_HPP
