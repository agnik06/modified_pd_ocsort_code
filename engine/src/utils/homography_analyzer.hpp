#ifndef __HOMOGRAPHY_ANALYZER__
#define __HOMOGRAPHY_ANALYZER__

#include <ctime>
#include <opencv2/opencv.hpp>
#include <vector>
#include "logger/logger.hpp"
//#define RUN
#define RESCALE_IMAGE_DIM 128
#define N_BINS 4 //perfect-squares are expected

class HomograhyAnalyzer {
 public:
  struct HomographyDescriptor{
    cv::Mat frame;
    cv::Mat descriptor;
    std::vector<cv::KeyPoint> keypoints;    
  };

  HomograhyAnalyzer();
  HomograhyAnalyzer(cv::Ptr<cv::Feature2D> detector, cv::Ptr<cv::DescriptorMatcher> matcher);
  void set_threshold(const float threshold) { this->threshold_ = threshold; }

  void set_reference_frame(const cv::Mat& reference_frame);
  bool has_reference_frame();
  float analyze(const cv::Mat& reference_frame, const cv::Mat& frame);
  float analyze(const cv::Mat& frame);

  void set_reference_frame_multiscale(const cv::Mat& reference_frame);
  bool has_reference_frame_multiscale();
  float analyze_multiscale(const cv::Mat& frame);

  float histogram_analyze();
  void clear();
  ~HomograhyAnalyzer();
 private:
  
  cv::Ptr<cv::Feature2D> detector_;
  cv::Ptr<cv::DescriptorMatcher> matcher_;

  HomographyDescriptor reference_descriptor;
  HomographyDescriptor current_descriptor;

  std::vector<HomographyDescriptor> reference_descriptors_multiscale;
  std::vector<HomographyDescriptor>   current_descriptors_multiscale;

  void set_homography_descriptor(cv::Mat frame, HomographyDescriptor& descriptor);  
  float compute_homography_distance(HomographyDescriptor h1, HomographyDescriptor h2);
  float compute_histogram_distance(HomographyDescriptor h1, HomographyDescriptor h2);

  void populate_bin_descriptors(const cv::Mat& frame, std::vector<HomographyDescriptor>& descriptors);

  // cv::Mat reference_frame_, frame_, reference_descriptor_;
  // std::vector<cv::KeyPoint> reference_keypoints_;

  float threshold_;
  float histogram_difference(const cv::Mat& image_a, const cv::Mat& image_b);
};
#endif
