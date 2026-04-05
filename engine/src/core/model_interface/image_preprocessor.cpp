#include "image_preprocessor.hpp" 
#include "logger/logger.hpp"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <cstdlib>

namespace vizpetinsight {

cv::Mat cropResizeCvtColorYUV(char* image, int width, int height, int left, int top, int right, int bottom, int target_width, int target_height) {
	LOGV("AJ Debug: cropResizeCvtColorYUVOpenV w:%d, h:%d, l:%d, t:%d, r:%d, b:%d, tw:%d, th:%d",width, height, left, top, right, bottom, target_width, target_height);
	LOGV("VZ Debug: cropResizeCvtColorYUVOpenCV called");
	auto start = std::chrono::steady_clock::now();

	cv::Mat bgr_image;  
	cv::Mat yuv(height+height/2, width, CV_8UC1, image);
	cv::Mat frame(height, width, CV_8UC3);
	cv::cvtColor(yuv, frame, cv::COLOR_YUV2BGRA_NV21); //CV_YUV2BGR_NV21 = 93,

	cv::cvtColor(frame, bgr_image, cv::COLOR_BGRA2BGR);
	cv::Rect roi(left, top, right - left, bottom - top);
	// cv::Mat cropped_image = bgr_image(roi);
	cv::Mat cropped_image = get_cropped_image(bgr_image, {left, top, right, bottom});

	cv::Mat resized_image;
	cv::resize(cropped_image, resized_image, cv::Size(target_width, target_height));
	cv::Mat float_image;
	resized_image.convertTo(float_image, CV_32FC3);

	auto finish = std::chrono::steady_clock::now();
	double elapsed_time = (std::chrono::duration_cast<std::chrono::microseconds>(finish - start)).count();
	elapsed_time /= 1000.0;
	LOGV("VZ_Debug: cropResizeCvtColorYUVOpenCV time taken: %f ms, target: %d, %d", elapsed_time, target_width, target_height);		
	LOGV("VZ Debug: cropResizeCvtColorYUVOpenCV completed");
	return float_image;
}
cv::Mat get_cropped_image(cv::Mat image, vizpet::Rectangle crop_rect) {
	vizpet::Rectangle crop_rect_org(crop_rect.left, crop_rect.top, crop_rect.right, crop_rect.bottom);
  cv::Mat crop_image;
  float width = crop_rect.right - crop_rect.left + 1;
  float height = crop_rect.bottom - crop_rect.top + 1;
  int width_inc = (int)(0.025 * width);
  int height_inc = (int)(0.025 * height);

  crop_rect.left = crop_rect.left - width_inc;
  crop_rect.right = crop_rect.right + width_inc;
  crop_rect.top = crop_rect.top - height_inc;
  crop_rect.bottom = crop_rect.bottom + height_inc;

  float aspect_ratio = float(crop_rect.right - crop_rect.left) /
                       float(crop_rect.bottom - crop_rect.top);
  if (aspect_ratio > 2.0) {
    int required_height = (crop_rect.right - crop_rect.left) / 2;
    int deficit = required_height - (crop_rect.bottom - crop_rect.top);
    crop_rect.bottom += (deficit * 0.5);
    crop_rect.top -= (deficit * 0.5);
    std::cout << "deficit:" << deficit << std::endl;
  }
  if (aspect_ratio < 0.5) {
    int required_width = (crop_rect.bottom - crop_rect.top) / 2;
    int deficit = required_width - (crop_rect.right - crop_rect.left);
    crop_rect.right += (deficit * 0.5);
    crop_rect.left -= (deficit * 0.5);
    std::cout << "deficit:" << deficit << std::endl;
  }
  if (crop_rect.right > image.cols - 1) {
    crop_rect.right = image.cols - 1;
  }
  if (crop_rect.bottom > image.rows - 1) {
    crop_rect.bottom = image.rows - 1;
  }
  if (crop_rect.left < 0) {
    crop_rect.left = 0;
  }
  if (crop_rect.top < 0) {
    crop_rect.top = 0;
  }
	
  cv::Rect roi(crop_rect.left, crop_rect.top,
               (crop_rect.right - crop_rect.left + 1),
               (crop_rect.bottom - crop_rect.top + 1));
  cv::Mat image_roi = image(roi);
  image_roi.copyTo(crop_image);

  return crop_image;
}

cv::Mat cropResizeBGR(char* image, int width, int height, int left, int top, int right, int bottom, int target_width, int target_height) {
	LOGV("AJ Debug: cropResizeCvtColorBGR w:%d, h:%d, l:%d, t:%d, r:%d, b:%d, tw:%d, th:%d",width, height, left, top, right, bottom, target_width, target_height);
	LOGV("VZ Debug: cropResizeCvtColorBGR called");
	// cv::Mat yuv(height+height/2, width, CV_8UC1, image);
	cv::Rect roi(left, top, right - left, bottom - top);
	cv::Size img_size(width, height);
	cv::Size op_size(target_width, target_height);
	cv::Mat output_image(img_size, CV_32FC3);

  cv::Mat image1(height, width, CV_8UC3, image);
	// cv::Mat image1;  
	// cv::Mat frame(height, width, CV_8UC3);
	// cv::cvtColor(yuv, frame, CV_YUV2BGRA_NV21); //CV_YUV2BGR_NV21 = 93,
	// cv::cvtColor(frame, image1, CV_BGRA2BGR);
	
	cv::Mat image2 = get_cropped_image(image1, {left, top, right, bottom});

  int n = rand();
  char f1[200],f2[200], f3[200];
  sprintf(f2, "Results/dump/%d_image1.jpg", n);
  sprintf(f1, "Results/dump/%d_image4.jpg", n);
  sprintf(f2, "Results/dump/%d_image2.jpg", n);

	cv::Mat image3;
	cv::resize(image2, image3, op_size);

	cv::Mat image4;
    image3.convertTo(image4, CV_32FC3);

	LOGV("VZ Debug: cropResizeCvtColorBGR completed");
	// imwrite(f1, image4);
	// imwrite(f2, image2);
	// imwrite(f3, image1);
	// return output_image;
	return image4;
}

}