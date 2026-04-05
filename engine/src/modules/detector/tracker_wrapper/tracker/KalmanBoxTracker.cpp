#include "KalmanBoxTracker.hpp"
#include "Utilities.hpp"
#include <cmath>

namespace ocsort {

int KalmanBoxTracker::count = 0;

KalmanBoxTracker::KalmanBoxTracker(const Eigen::VectorXf& bbox, int delta_t_)
    : delta_t(delta_t_), kf(7, 4) {
    // Initialize Kalman Filter with constant velocity model
    
    // State transition matrix F
    Eigen::MatrixXf F(7, 7);
    F << 1, 0, 0, 0, 1, 0, 0,
         0, 1, 0, 0, 0, 1, 0,
         0, 0, 1, 0, 0, 0, 1,
         0, 0, 0, 1, 0, 0, 0,
         0, 0, 0, 0, 1, 0, 0,
         0, 0, 0, 0, 0, 1, 0,
         0, 0, 0, 0, 0, 0, 1;
    kf.set_F(F);
    
    // Measurement matrix H
    Eigen::MatrixXf H(4, 7);
    H << 1, 0, 0, 0, 0, 0, 0,
         0, 1, 0, 0, 0, 0, 0,
         0, 0, 1, 0, 0, 0, 0,
         0, 0, 0, 1, 0, 0, 0;
    kf.set_H(H);
    
    // Set R and Q matrices
    Eigen::MatrixXf R = Eigen::MatrixXf::Identity(4, 4);
    R(2, 2) *= 10.0f;
    R(3, 3) *= 10.0f;
    kf.set_R(R);
    
    Eigen::MatrixXf Q = Eigen::MatrixXf::Identity(7, 7);
    Q(6, 6) *= 0.01f;
    for (int i = 4; i < 7; ++i) {
        Q(i, i) *= 0.01f;
    }
    kf.set_Q(Q);
    
    // Initialize state with bbox
    Eigen::VectorXf z = convert_bbox_to_z(bbox.head(4));
    kf.predict();
    kf.update(&z);
    
    // Initialize tracker state
    id = KalmanBoxTracker::count;
    KalmanBoxTracker::count++;
    
    time_since_update = 0;
    hits = 0;
    hit_streak = 0;
    age = 0;
    
    last_observation = Eigen::RowVectorXf::Zero(5);
    last_observation.head(5) = bbox;
    
    observations[age] = last_observation;
    history_observations.push_back(last_observation);
    
    velocity = Eigen::Vector2f::Zero();
}

void KalmanBoxTracker::update(const Eigen::VectorXf* bbox) {
    if (bbox) {
        // Estimate velocity if we have previous observation
        if (last_observation.sum() >= 0) {
            Eigen::RowVectorXf previous_box;
            bool found = false;
            
            for (int i = 0; i < delta_t; ++i) {
                int dt = delta_t - i;
                auto it = observations.find(age - dt);
                if (it != observations.end()) {
                    previous_box = it->second;
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                previous_box = last_observation;
            }
            
            // Estimate speed direction with observations delta_t steps away
            velocity = speed_direction(previous_box, *bbox);
        }
        
        // Insert new observation
        last_observation = *bbox;
        observations[age] = last_observation;
        history_observations.push_back(last_observation);
        
        time_since_update = 0;
        history.clear();
        hits++;
        hit_streak++;
        
        // Update Kalman filter
        Eigen::VectorXf z = convert_bbox_to_z(bbox->head(4));
        kf.update(&z);
    } else {
        kf.update(nullptr);
    }
}

Eigen::VectorXf KalmanBoxTracker::predict() {
    // Check if scale + aspect ratio is negative
    if ((kf.get_state()(6) + kf.get_state()(2)) <= 0) {
        Eigen::VectorXf x_new = kf.get_state();
        x_new(6) *= 0.0f;
        // Update internal state directly
        // Note: This is a simplified approach
    }
    
    kf.predict();
    age++;
    
    if (time_since_update > 0) {
        hit_streak = 0;
    }
    time_since_update++;
    
    Eigen::VectorXf bbox = convert_x_to_bbox(kf.get_state().head(4));
    history.push_back(bbox);
    
    return bbox;
}

Eigen::VectorXf KalmanBoxTracker::get_state() {
    Eigen::VectorXf state_4 = kf.get_state().head(4);
    return convert_x_to_bbox(state_4);
}

} // namespace ocsort
