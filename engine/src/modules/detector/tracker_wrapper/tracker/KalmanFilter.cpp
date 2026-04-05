#include "KalmanFilter.hpp"

namespace ocsort {

KalmanFilter::KalmanFilter(int dim_x_, int dim_z_) 
    : dim_x(dim_x_), dim_z(dim_z_) {
    x = Eigen::VectorXf::Zero(dim_x);
    P = Eigen::MatrixXf::Identity(dim_x, dim_x);
    Q = Eigen::MatrixXf::Identity(dim_x, dim_x);
    R = Eigen::MatrixXf::Identity(dim_z, dim_z);
    F = Eigen::MatrixXf::Identity(dim_x, dim_x);
    H = Eigen::MatrixXf::Zero(dim_z, dim_x);
    
    y = Eigen::VectorXf::Zero(dim_z);
    S = Eigen::MatrixXf::Zero(dim_z, dim_z);
    K = Eigen::MatrixXf::Zero(dim_x, dim_z);
    I = Eigen::MatrixXf::Identity(dim_x, dim_x);
}

void KalmanFilter::predict(const Eigen::MatrixXf* B, const Eigen::VectorXf* u,
                           const Eigen::MatrixXf* F_in, const Eigen::MatrixXf* Q_in) {
    const Eigen::MatrixXf& F_use = F_in ? *F_in : F;
    const Eigen::MatrixXf& Q_use = Q_in ? *Q_in : Q;
    
    // x = Fx + Bu
    if (B && u) {
        x = F_use * x + (*B) * (*u);
    } else {
        x = F_use * x;
    }
    
    // P = FPF' + Q
    P = F_use * P * F_use.transpose() + Q_use;
}

void KalmanFilter::update(const Eigen::VectorXf* z, const Eigen::MatrixXf* R_in,
                          const Eigen::MatrixXf* H_in) {
    const Eigen::MatrixXf& R_use = R_in ? *R_in : R;
    const Eigen::MatrixXf& H_use = H_in ? *H_in : H;
    
    if (z) {
        // y = z - Hx
        y = *z - H_use * x;
        
        // S = HPH' + R
        S = H_use * P * H_use.transpose() + R_use;
        
        // K = PH'S^(-1)
        K = P * H_use.transpose() * S.inverse();
        
        // x = x + Ky
        x = x + K * y;
        
        // P = (I - KH)P(I - KH)' + KRK'
        Eigen::MatrixXf I_KH = I - K * H_use;
        P = I_KH * P * I_KH.transpose() + K * R_use * K.transpose();
    }
}

} // namespace ocsort
