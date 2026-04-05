/**
 * @file   detector.hpp
 * @brief  This file provides base interface for detector
 *
 * This class provides base interface out of which detector is implemented.
 */

#ifndef __DETECTOR_INTERFACE__
#define __DETECTOR_INTERFACE__

#include <vector>
#include <opencv2/opencv.hpp>

#include "framework/bounding_box.hpp"
#include "net/roi_tensor_parser.hpp"

class IDetector{
public:
    virtual ~IDetector(){};
    virtual std::vector<vizpet::BoundingBox> detect(cv::Mat image) = 0;
    virtual void set_tensor_parser(ROITensorParser tensor_parser) = 0;
    virtual void set_float_parser(ROIFloatParser float_parser) = 0;
    virtual void get_target_dimen(int& target_width,int& target_height)=0;
    virtual void set_detection_out_layer(std::vector<std::string> layer_name)=0;

    std::vector<std::string> output_layer_name;
};

#endif //__DETECTOR_INTERFACE__