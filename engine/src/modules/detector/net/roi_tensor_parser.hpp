/**
 * @file   roi_tensor_parser.hpp
 * @brief  This file provides interfaces for getting the parsed ssd tensor
 *
 * Interfaces for getting the parsed ssd tensor
 */
#ifndef __TENSOR_PARSER__
#define __TENSOR_PARSER__

#include "logger/logger.hpp"
#include "framework/tensor.hpp"
#include "framework/bounding_box.hpp"
#include "framework/rectangle.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <functional>

typedef std::function<std::vector<vizpet::BoundingBox> (Tensor3D<float> tensor, cv::Mat input_image)> ROITensorParser;
typedef std::function<std::vector<vizpet::BoundingBox> (std::vector<std::vector<float> > roi, cv::Mat input_image)> ROIFloatParser;
/**
 * @brief gets the ssd tensor parser
 *
 * @param[in] none
 *
 * @return callback for ssd roi tensor
 */
ROITensorParser get_ssd_tensor_parser();
ROIFloatParser get_2D_tensor_parser();

#endif //__TENSOR_PARSER__