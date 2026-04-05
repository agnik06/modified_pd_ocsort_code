/**
 * @file   image_utils.hpp
 * @brief  This file provides interfaces for different image utilities
 *
 * This file provides interfaces for different image utilities
 */

#ifndef __IMAGE_UTILS__
#define __IMAGE_UTILS__

#include <opencv2/opencv.hpp>

#include "pet_detector.h"
#include <string>
#include "logger/timer.hpp"
#include "image_utils_neon.hpp"
#include "framework/bounding_box.hpp"
#include "rectangle_utils.hpp"

/**
 * @brief Checks if an image is dark or not.
 * If the image is totally dark models are not executed and it saves computation time.
 * 
 * @param[in] image
 *
 * @return bool true if dark, false otherwise
 */
bool is_image_dark(cv::Mat image);
/**
 * @brief Get bgr mat from image buffer
 * 
 * @param[in] bitmap buffer of image
 * @param[in] width of image
 * @param[in] height of image
 * @param[in] Image format
 * @param[out] target image width
 * @param[out] target image height
 *
 * @return Converted image 
 */
cv::Mat get_bgr_mat_from_buffer(char* bitmap_buffer, int width, int height, const vizpet::PetDetector::ImageFormat format,int& target_width,int& target_height);
/**
 * @brief Rotate given image clockwise based on angle
 * 
 * @param[in] Image
 * @param[in] angle to rotate with
 *
 * @return Rotated image
 */
cv::Mat rotate_image(cv::Mat image, int clockwise_angle);
/**
 * @brief Image with exif correction. exif stores the meta data of image
 * 
 * @param[in] Image
 * @param[in] orientation
 *
 * @return exif corrected image
 */
cv::Mat exif_correction(cv::Mat image, const int orientation);
/**
 * @brief Get image rect with 4:3 center crop
 * 
 * @param[in] Image
 *
 * @return exif corrected image
 */
cv::Rect get_4_3_center_crop_rect(cv::Mat image);
void fill_mask_buffer(std::string& buffer, cv::Mat mask);
cv::Vec3b get_bgr_pixel_yuv(char* yuv, const int image_width, int const image_height, int i, int j);
cv::Vec3b get_bgr_pixel_yvu(char* yuv, const int image_width, int const image_height, int i, int j);
/**
 * @brief Resize and rotate the given image
 * 
 * @param[in] Image buffer with yuv format
 * @param[in] input image width
 * @param[in] input image height
 * @param[in] converted image width
 * @param[in] converted image height
 * @param[in] rotated angle
 * @param[in] image format
 *
 * @return Rotated Image
 */
cv::Mat nv_convert_resize_rotate(char* yuv, const int image_width, 
                                    int const image_height, int const target_width, 
                                    int const target_height, int rotation_angle, 
                                    const vizpet::PetDetector::ImageFormat format);

cv::Mat normalize_image(cv::Mat uchar_image, std::vector<float> means, std::vector<float> scales);

std::vector<vizpet::BoundingBox> merge_pet_box_inside_petbox(std::vector<vizpet::BoundingBox> boxes);
cv::Mat stabilize_for_focus(const cv::Mat& input_image, double sigma, double amount);
cv::Mat stabilize_for_illumination(const cv::Mat& input_image);

#endif //__IMAGE_UTILS__