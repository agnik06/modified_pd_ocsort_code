/**
 * @file   default_roi_mapper.cpp
 * @brief  This file defines the mapping of boxes with labels
 *
 * This provides the interfaces for assigning the boxes with labels
 */

#include "default_roi_mapper.hpp"

DefaultROIMapper::DefaultROIMapper(std::shared_ptr<ROIDataHandler> data_handler)
    :IROIMapper(data_handler){
}
DefaultROIMapper::~DefaultROIMapper(){

}
std::vector<vizpet::BoundingBox> DefaultROIMapper::map(std::vector<vizpet::BoundingBox> boxes){
    boxes = assign_labels(boxes);
    return boxes;
}

std::vector<vizpet::BoundingBox> DefaultROIMapper::assign_labels(std::vector<vizpet::BoundingBox> boxes){
    for(int i=0; i<boxes.size(); i++){
        boxes[i].tag = roi_data_handler->get_label(boxes[i].tag_id);
    }
    return boxes;
}
