#ifndef KALMANFILTER_HPP
#define KALMANFILTER_HPP

#include <Eigen/Dense>
#include <vector>

namespace ocsort {

class KalmanFilter {
public:
    KalmanFilter(int dim_x, int dim_z);
    
    // Predict next state
    void predict(const Eigen::MatrixXf* B = nullptr, const Eigen::VectorXf* u = nullptr,
                 const Eigen::MatrixXf* F = nullptr, const Eigen::MatrixXf* Q = nullptr);
    
    // Update with measurement
    void update(const Eigen::VectorXf* z, const Eigen::MatrixXf* R = nullptr,
                const Eigen::MatrixXf* H = nullptr);
    
    // Getters
    inline Eigen::VectorXf get_state() const { return x; }
    inline Eigen::MatrixXf get_covariance() const { return P; }
    inline int get_dim_x() const { return dim_x; }
    inline int get_dim_z() const { return dim_z; }
    
    // Setters
    inline void set_F(const Eigen::MatrixXf& F_) { F = F_; }
    inline void set_H(const Eigen::MatrixXf& H_) { H = H_; }
    inline void set_R(const Eigen::MatrixXf& R_) { R = R_; }
    inline void set_Q(const Eigen::MatrixXf& Q_) { Q = Q_; }
    inline void set_state(const Eigen::VectorXf& x_) { x = x_; }
    
private:
    int dim_x;
    int dim_z;
    
    Eigen::VectorXf x;        // State estimate
    Eigen::MatrixXf P;        // State covariance
    Eigen::MatrixXf Q;        // Process noise covariance
    Eigen::MatrixXf R;        // Measurement noise covariance
    Eigen::MatrixXf F;        // State transition matrix
    Eigen::MatrixXf H;        // Measurement matrix
    
    Eigen::VectorXf y;        // Residual
    Eigen::MatrixXf S;        // System uncertainty
    Eigen::MatrixXf K;        // Kalman gain
    Eigen::MatrixXf I;        // Identity matrix
};

} // namespace ocsort

#endif // KALMANFILTER_HPP
