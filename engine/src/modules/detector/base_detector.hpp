/**
 * @file   base_detector.hpp
 * @brief  This file provides implementation of BaseDetector
 *
 * This class provides for implemetation of BaseDetector.
 */

#ifndef __BASE_DETECTOR__
#define __BASE_DETECTOR__

#include "detector.hpp"
#include "net/detector_net.hpp"
#include <memory>

class BaseDetector: public IDetector{
public:
    BaseDetector(std::shared_ptr<DetectorNet> detector_net);
    ~BaseDetector();
    /**
     * @brief Detects bounding boxes for given image
     *
     * @param[in] Image for which the bounding box is detected
     *
     * @return Returns bounding boxes for given image
     */
    std::vector<vizpet::BoundingBox> detect(cv::Mat image);
    /**
     * @brief Sets the tensor
     *
     * @param[in] tensor_parser to set
     *
     * @return void
     */
    void set_tensor_parser(ROITensorParser tensor_parser);
    /**
     * @brief Sets the tensor
     *
     * @param[in] float_parset to set
     *
     * @return void
     */
    void set_float_parser(ROIFloatParser float_parser);
    /**
     * @brief Gets the target image width and height
     *
     * @param[in] target_width
     * @param[in] target_height
     *
     * @return none
     */
    void get_target_dimen(int& target_width,int& target_height);

    void set_detection_out_layer(std::vector<std::string> layer_name);

protected:
    std::shared_ptr<DetectorNet> detector_net;
    ROITensorParser tensor_parser;
    ROIFloatParser float_parser;
    
};

#endif //__BASE_DETECTOR__
