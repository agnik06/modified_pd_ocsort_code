/**
 * @file   base_detector.cpp
 * @brief  This file implements the logic for the base_detector
 *
 * Implements the logic for the base_detector
 */
#include "base_detector.hpp"
#include "net/roi_tensor_parser.hpp"
#include "logger/logger.hpp"

BaseDetector::BaseDetector(std::shared_ptr<DetectorNet> detector_net)
    :detector_net(detector_net){
    this->tensor_parser = get_ssd_tensor_parser();
}

BaseDetector::~BaseDetector(){

}

/**
 * @brief Detects bounding boxes for given image
 *
 * @param[in] Image for which the bounding box is detected
 *
 * @return Returns bounding boxes for given image
 */
std::vector<vizpet::BoundingBox> BaseDetector::detect(cv::Mat image){
    bool forward_pass_status = detector_net->forward_pass(image);;
    if (!forward_pass_status){
        LG << "VZ Debug: Forwards pass failed";
        return {};
    }
    //LOGV("VZ Debug: output_layer_name  - ", this->output_layer_name[0]);

    if(this->output_layer_name[0] == "output"){ //for v1
    #ifdef DUMPBOX
        LOGV("VZ Debug: detect In output");
    #endif
        // LOGV("VZ Debug: detect In get_output_customlayer");
        std::vector<std::vector<float> > output_roi = detector_net->get_output_customlayer("output");
        // LOGV("VZ Debug: output_roi size - [%lu]", output_roi.size());
        std::vector<vizpet::BoundingBox> boxes = float_parser(output_roi, image);
        // LOGV("VZ Debug: # of boxes detected 1: [%lu] ", boxes.size());
        return boxes;
    }else if (this->output_layer_name[0] =="Identity"){
    #ifdef DUMPBOX
        LOGV("VZ Debug: detect In Identity");
    #endif
        std::vector<std::vector<float> > output_roi = detector_net->get_output_customlayer("Identity");
        //LOGV("VZ Debug: output_roi size - [%lu]", output_roi.size());
        std::vector<vizpet::BoundingBox> boxes = float_parser(output_roi, image);
        //LOGV("VZ Debug: # of boxes detected 1: [%lu] ", boxes.size());
        return boxes;
    }
    else if (this->output_layer_name.size() == 2){ //for aicod where there are 2 layers
        //LG<<"VZ Debug: detect with 2 layers";
        std::vector<std::vector<float> > output_roi = detector_net->get_output_custom_decode_with_nms("mbox_loc", "mbox_conf");
        std::vector<vizpet::BoundingBox> boxes = float_parser(output_roi, image);
        #ifdef DUMPBOX
            LOGV("VZ Debug: # of boxes detected 2 : [%lu]", boxes.size());
        #endif
        return boxes;
    }
    else { //for v2 and v1
        //LG<<"VZ Debug: detect with 1 layers";
        Tensor3D<float> output_tensor = detector_net->get_output("detection_out");
        std::vector<vizpet::BoundingBox> boxes = tensor_parser(output_tensor, image);
        #ifdef DUMPBOX
            LOGV("VZ Debug: # of boxes detected 3: [%lu]", boxes.size());
        #endif
        return boxes;
    }
}

/**
 * @brief Sets the tensor
 *
 * @param[in] tensor_parser to set
 *
 * @return void
 */
void BaseDetector::set_tensor_parser(ROITensorParser tensor_parser){
    this->tensor_parser = tensor_parser;
}

/**
 * @brief Sets the tensor
 *
 * @param[in] float_parser to set
 *
 * @return void
 */
void BaseDetector::set_float_parser(ROIFloatParser float_parser){
    this->float_parser = float_parser;
}

/**
 * @brief Sets the set_detection_out_layer
 *
 * @param[in] set_detection_out_layer to set
 *
 * @return void
 */
void BaseDetector::set_detection_out_layer(std::vector<std::string> layer_name){
    for (int i=0; i<layer_name.size(); i++){
        this->output_layer_name.push_back(layer_name[i]);
    }
}

/**
 * @brief Gets the target image width and height
 *
 * @param[in] target_width
 * @param[in] target_height
 *
 * @return none
 */
void BaseDetector::get_target_dimen(int& target_width,int& target_height){
    this->detector_net->get_target_dimen(target_width,target_height);
}
