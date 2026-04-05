/**
 * @file   base_tagger.cpp
 * @brief  This file provides implementation of BaseTagger
 *
 * This class provides for implemetation of BaseTagger.
 */

#include <logger/logger.hpp>
#include "base_tagger.hpp"
#include "hooks/entity_hooks.hpp"

BaseTagger::BaseTagger(){

}
BaseTagger::~BaseTagger(){

}
std::vector<vizpet::BoundingBox> BaseTagger::get_boxes(cv::Mat image){
    std::vector<vizpet::BoundingBox> boxes;
    if(this->detector != nullptr){
        boxes = this->detector->detect(image);
    }
    if(this->roi_mapper != nullptr){
        boxes = this->roi_mapper->map(boxes);
    }
    if(this->roi_filterer != nullptr){
        boxes = this->roi_filterer->filter(boxes, image);
    }
    return boxes;
}

void BaseTagger:: get_target_dimen(int& t_height,int& t_width){
    (this->detector)->get_target_dimen(t_height,t_width);
}

void BaseTagger::set_detector(std::shared_ptr<IDetector> detector){
    this->detector = detector;
}

void BaseTagger::set_roi_mapper(std::shared_ptr<IROIMapper> roi_mapper){
    this->roi_mapper = roi_mapper;
}

void BaseTagger::set_roi_filterer(std::shared_ptr<IROIFilterer> roi_filterer){
    this->roi_filterer = roi_filterer;
}