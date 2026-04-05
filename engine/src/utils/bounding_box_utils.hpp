/**
 * @file   bounding_box_utils.hpp
 * @brief  This file provides interfaces for different bounding box utilities
 *
 * This file provides interfaces for different bounding box utilities
 */

#ifndef __BOUNDING_BOX_UTILS__
#define __BOUNDING_BOX_UTILS__

#include "core/framework/bounding_box.hpp"
#include <opencv2/opencv.hpp>

double calculateIoU(const vizpet::Rectangle& box1, const vizpet::Rectangle& box2);
std::vector<vizpet::BoundingBox> mergeBoundingBoxes(const std::vector<vizpet::BoundingBox>& boxes, double iouThreshold);
vizpet::Rectangle update_smother_box(const vizpet::Rectangle& newBox,vizpet::Rectangle& smother_box);
double calculateWeight(const vizpet::Rectangle& box, vizpet::Rectangle& smother_box);
vizpet::Rectangle update_weighted_smoother_box(const vizpet::Rectangle& newBox,vizpet::Rectangle& smother_box);
cv::Mat stabilizeFocus(const cv::Mat& inputImage);
cv::Mat stabilizeLighting(const cv::Mat& inputImage);
double calculateLaplacianVariance(const cv::Mat& frame);
std::vector<vizpet::BoundingBox> adptiveMergeBoundingBoxes(const std::vector<vizpet::BoundingBox>& boxes);
//std::vector<vizpet::BoundingBox> checkAndRemoveBoxes(std::vector<vizpet::BoundingBox>& boxes);
bool isInside (vizpet::BoundingBox inner, vizpet::BoundingBox outer);
void removeNestedBoxes (std::vector<vizpet::BoundingBox>& boxes);

#endif //__BOUNDING_BOX_UTILS__