#include "homography_analyzer.hpp"

HomograhyAnalyzer::HomograhyAnalyzer() {
  cv::Ptr<cv::AKAZE> akaze = cv::AKAZE::create();
  cv::Ptr<cv::DescriptorMatcher> matcher =
      cv::DescriptorMatcher::create("BruteForce-Hamming");
  this->detector_ = akaze;
  this->matcher_ = matcher;
  this->threshold_ = 0.25;
  this->reference_descriptors_multiscale.resize(N_BINS);
  this->current_descriptors_multiscale.resize(N_BINS);
}
HomograhyAnalyzer::HomograhyAnalyzer(cv::Ptr<cv::Feature2D> detector,
                                     cv::Ptr<cv::DescriptorMatcher> matcher) {
  this->detector_ = detector;
  this->matcher_ = matcher;
  this->threshold_ = 0.25;
  this->reference_descriptors_multiscale.resize(N_BINS);
  this->current_descriptors_multiscale.resize(N_BINS);
}

HomograhyAnalyzer::~HomograhyAnalyzer() {
  this->detector_.release();
  this->matcher_.release();
}

void HomograhyAnalyzer::set_homography_descriptor(cv::Mat frame, HomographyDescriptor& descriptor){
  if(frame.rows == RESCALE_IMAGE_DIM && frame.cols == RESCALE_IMAGE_DIM){
    descriptor.frame = frame;
  }
  else{
    cv::resize(frame, descriptor.frame, cv::Size(RESCALE_IMAGE_DIM, RESCALE_IMAGE_DIM));
  }
  detector_->detectAndCompute(descriptor.frame, cv::noArray(), descriptor.keypoints, descriptor.descriptor);
}

void HomograhyAnalyzer::set_reference_frame(const cv::Mat& reference_frame) {
  set_homography_descriptor(reference_frame, reference_descriptor);
}
bool HomograhyAnalyzer::has_reference_frame() {
  return reference_descriptor.frame.rows > 0;
}

float HomograhyAnalyzer::compute_homography_distance(HomographyDescriptor h1, HomographyDescriptor h2){
  const double ransac_thresh = 2.8f;   // RANSAC inlier threshold
  const double nn_match_ratio = 0.7f;  // Nearest-neighbour matching ratio

  std::vector<std::vector<cv::DMatch> > matches;
  matcher_->knnMatch(h1.descriptor, h2.descriptor, matches, 2);
  std::vector<cv::Point> matched1, matched2;
  std::vector<cv::DMatch> good_matches;

  for (unsigned i = 0; i < matches.size(); i++) {
    if (matches[i][0].distance < nn_match_ratio * matches[i][1].distance) {
      good_matches.push_back(matches[i][0]);
      if (matches[i][0].queryIdx >= 0 &&
          matches[i][0].queryIdx < h1.keypoints.size()) {
        matched1.push_back(h1.keypoints[matches[i][0].queryIdx].pt);
        if (matches[i][0].trainIdx >= 0 &&
            matches[i][0].trainIdx < h2.keypoints.size()) {
          matched2.push_back(h2.keypoints[matches[i][0].trainIdx].pt);
        }
      }
    }
  }

#ifdef RUN
  cv::Mat img_matches;
  cv::drawMatches(reference_frame_, h1.keypoints, h2.frame, h2.keypoints,
                  good_matches, img_matches, cv::Scalar::all(-1),
                  cv::Scalar::all(-1), std::vector<char>(),
                  cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

  cv::imshow("matches", img_matches);
  cv::waitKey(0);
#endif
  cv::Mat inlier_mask, homography;
  float homography_distance = 0.0;
  if (matched1.size() >= 4 && matched2.size() >= 4) {
    try {
      homography = cv::findHomography(matched1, matched2, cv::RANSAC,
                                      ransac_thresh, inlier_mask);
    } catch (cv::Exception& e) {
      return 1.0f;
    }

    for (int i = 0; i < homography.rows; ++i) {
      for (int j = 0; j < homography.cols; ++j) {
        if (j == i) {  // Difference from idenity
          homography_distance += std::abs(1.0 - homography.at<double>(i, j)); 
        }
        else if (j == 2) {  // Normalize translation
          homography_distance += std::abs(homography.at<double>(i, j)) / RESCALE_IMAGE_DIM;
        }
        else {  // add absolute value of rotational/projection components
          homography_distance += std::abs(homography.at<double>(i, j)); 
        }
      }
    }

    homography_distance /= 9.0;
    homography_distance = std::min<float>(1.0, homography_distance);

  } else {
    // std::cout << "No matches found." << std::endl;
    homography_distance = 1.0;
  }
  return homography_distance;
}

float HomograhyAnalyzer::compute_histogram_distance(HomographyDescriptor h1, HomographyDescriptor h2){
  cv::Mat image_a = h1.frame;
  cv::Mat image_b = h2.frame;
  float hist_distance = 0.0;
  int window_size = 32;
  int step = 32;
  int total_bins = 0;
  float fail_threshold = 0.25;
  for (int row = 0; row <= image_a.rows - window_size; row += step) {
    for (int col = 0; col <= image_a.rows - window_size; col += step) {
      cv::Rect window(col, row, window_size, window_size);
      cv::Mat image_a_roi = cv::Mat(image_a, window);
      cv::Mat image_b_roi = cv::Mat(image_b, window);
      float roi_hist_distance = histogram_difference(image_a_roi, image_b_roi);
#ifdef VOTING
      if (roi_hist_distance > fail_threshold) {
        hist_distance += 1;
      }
#else
      hist_distance += roi_hist_distance;
#endif
      total_bins += 1;
    }
  }
  hist_distance = hist_distance / total_bins;
  return hist_distance;
}

float HomograhyAnalyzer::analyze(const cv::Mat& reference_frame, const cv::Mat& frame) {
  set_reference_frame(reference_frame);
  return analyze(frame);
}

float HomograhyAnalyzer::analyze(const cv::Mat& frame) {
  set_homography_descriptor(frame, current_descriptor);

  float homography_distance = compute_homography_distance(reference_descriptor, current_descriptor);
  float histogram_distance  = compute_histogram_distance(reference_descriptor, current_descriptor);

  float total_distance = homography_distance * histogram_distance;
  return total_distance;
}

void HomograhyAnalyzer::populate_bin_descriptors(const cv::Mat& frame, std::vector<HomographyDescriptor>& descriptors){
  int n = sqrt(N_BINS);
  cv::Mat resized_frame_;
  cv::resize(frame, resized_frame_, cv::Size(n*RESCALE_IMAGE_DIM, n*RESCALE_IMAGE_DIM));
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      int bin_id = i * n + j;
      int l =  i    * RESCALE_IMAGE_DIM;
      int t =  j    * RESCALE_IMAGE_DIM;
      cv::Mat bin_frame = resized_frame_({l,t,RESCALE_IMAGE_DIM, RESCALE_IMAGE_DIM});
      set_homography_descriptor(bin_frame, descriptors[bin_id]);
    }
  }
}

void HomograhyAnalyzer::set_reference_frame_multiscale(const cv::Mat& frame){
  populate_bin_descriptors(frame, reference_descriptors_multiscale);
}

bool HomograhyAnalyzer::has_reference_frame_multiscale(){
  if(!reference_descriptors_multiscale.empty()){
    if(reference_descriptors_multiscale[0].frame.rows != 0){
      return true;
    }
  }
  return false;
}

float HomograhyAnalyzer::analyze_multiscale(const cv::Mat& frame){
  populate_bin_descriptors(frame, current_descriptors_multiscale);
  float max_distance = 0.0f;
  for(int bin_id=0; bin_id<N_BINS; bin_id++){
    float homography_distance = compute_homography_distance(reference_descriptors_multiscale[bin_id], current_descriptors_multiscale[bin_id]);
    float histogram_distance  = compute_histogram_distance(reference_descriptors_multiscale[bin_id], current_descriptors_multiscale[bin_id]);
    float distance = homography_distance * histogram_distance;
    // LOGV("### bin: %d h1: %f, h2: %f", bin_id, homography_distance, histogram_distance);
    if(distance > max_distance){
      max_distance = distance;
    }
  }
  return max_distance;
}

float HomograhyAnalyzer::histogram_difference(const cv::Mat& image_a,
                                              const cv::Mat& image_b) {
  cv::Mat hsv_a;
  cv::Mat hsv_b;

  cv::cvtColor(image_a, hsv_a, cv::COLOR_BGR2HSV);
  cv::cvtColor(image_b, hsv_b, cv::COLOR_BGR2HSV);

  cv::Scalar mean, dev;
  cv::meanStdDev(hsv_a, mean, dev);
  hsv_a -= mean;  // to handle global image changes.

  cv::meanStdDev(hsv_b, mean, dev);
  hsv_b -= mean;  ////to handle global image changes.

  int h_bins = 50;
  int s_bins = 60;
  int histSize[] = {h_bins, s_bins};
  float h_ranges[] = {0, 180};
  float s_ranges[] = {0, 256};
  const float* ranges[] = {h_ranges, s_ranges};
  int channels[] = {0, 1};

  cv::MatND hist_a;
  cv::MatND hist_b;

  cv::calcHist(&hsv_a, 1, channels, cv::Mat(), hist_a, 2, histSize, ranges,
               true, false);
  cv::normalize(hist_a, hist_a, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  cv::calcHist(&hsv_b, 1, channels, cv::Mat(), hist_b, 2, histSize, ranges,
               true, false);
  cv::normalize(hist_b, hist_b, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  float distance = cv::compareHist(hist_a, hist_b, cv::HISTCMP_BHATTACHARYYA);
  return distance;
}

float HomograhyAnalyzer::histogram_analyze(){
  return compute_histogram_distance(reference_descriptor, current_descriptor);
}

void HomograhyAnalyzer::clear(){
  reference_descriptor = {};
  // reference_descriptors_multiscale.clear();
  reference_descriptors_multiscale.assign(N_BINS, {});

}
