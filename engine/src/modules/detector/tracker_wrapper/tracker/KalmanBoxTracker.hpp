#ifndef KALMANBOXTRACKER_HPP
#define KALMANBOXTRACKER_HPP

#include "KalmanFilter.hpp"
#include <Eigen/Dense>
#include <vector>
#include <map>

namespace ocsort {

class KalmanBoxTracker {
public:
    static int count;
    
    KalmanBoxTracker(const Eigen::VectorXf& bbox, int delta_t = 3);
    
    // Update with new measurement
    void update(const Eigen::VectorXf* bbox);
    
    // Predict next state
    Eigen::VectorXf predict();
    
    // Get current state
    Eigen::VectorXf get_state();
    
    // Public members
    int id;
    int time_since_update;
    int hits;
    int hit_streak;
    int age;
    Eigen::RowVectorXf last_observation;
    std::map<int, Eigen::RowVectorXf> observations;
    std::vector<Eigen::RowVectorXf> history_observations;
    Eigen::Vector2f velocity;
    int delta_t;
    
private:
    KalmanFilter kf;
    std::vector<Eigen::VectorXf> history;
};

} // namespace ocsort

#endif // KALMANBOXTRACKER_HPP
