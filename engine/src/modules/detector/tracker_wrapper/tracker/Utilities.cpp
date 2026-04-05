#include "Utilities.hpp"
#include <cmath>

namespace ocsort {

Eigen::VectorXf convert_bbox_to_z(const Eigen::VectorXf& bbox) {
    float w = bbox(2) - bbox(0);
    float h = bbox(3) - bbox(1);
    float x = bbox(0) + w / 2.0f;
    float y = bbox(1) + h / 2.0f;
    float s = w * h;
    float r = w / (h + 1e-6f);
    
    Eigen::VectorXf result(4);
    result << x, y, s, r;
    return result;
}

Eigen::VectorXf convert_x_to_bbox(const Eigen::VectorXf& x, float score) {
    float w = std::sqrt(x(2) * x(3));
    float h = x(2) / w;
    
    Eigen::VectorXf result(4);
    result << x(0) - w / 2.0f, x(1) - h / 2.0f, 
            x(0) + w / 2.0f, x(1) + h / 2.0f;
    return result;
}

Eigen::Vector2f speed_direction(const Eigen::RowVectorXf& bbox1, const Eigen::VectorXf& bbox2) {
    float cx1 = (bbox1(0) + bbox1(2)) / 2.0f;
    float cy1 = (bbox1(1) + bbox1(3)) / 2.0f;
    float cx2 = (bbox2(0) + bbox2(2)) / 2.0f;
    float cy2 = (bbox2(1) + bbox2(3)) / 2.0f;
    
    Eigen::Vector2f speed(cy2 - cy1, cx2 - cx1);
    float norm = std::sqrt((cy2 - cy1) * (cy2 - cy1) + (cx2 - cx1) * (cx2 - cx1)) + 1e-6f;
    
    return speed / norm;
}

Eigen::VectorXf k_previous_obs(const std::map<int, Eigen::RowVectorXf>& observations, 
                                 int cur_age, int k) {
    if (observations.empty()) {
        return Eigen::VectorXf::Zero(5);
    }
    
    for (int i = 0; i < k; ++i) {
        int dt = k - i;
        auto it = observations.find(cur_age - dt);
        if (it != observations.end()) {
            return it->second.transpose();
        }
    }
    
    // Return the observation with maximum age
    auto it = std::max_element(observations.begin(), observations.end(),
                               [](const auto& a, const auto& b) {
                                   return a.first < b.first;
                               });
    if (it != observations.end()) {
        return it->second.transpose();
    }
    
    return Eigen::VectorXf::Zero(5);
}

} // namespace ocsort
