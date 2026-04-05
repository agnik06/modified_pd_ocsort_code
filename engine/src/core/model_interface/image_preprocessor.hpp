#ifndef IMAGE_PREPROCESSOR
#define IMAGE_PREPROCESSOR

#include <opencv2/opencv.hpp>
#include "framework/rectangle.hpp"
namespace vizpetinsight {
cv::Mat get_cropped_image(cv::Mat image, vizpet::Rectangle crop_rect);
cv::Mat cropResizeCvtColorYUV(char* image, int width, int height, int left, int top, int right, int bottom, int target_width, int target_height);
cv::Mat cropResizeBGR(char* image, int width, int height, int left, int top, int right, int bottom, int target_width, int target_height);

}
#endif