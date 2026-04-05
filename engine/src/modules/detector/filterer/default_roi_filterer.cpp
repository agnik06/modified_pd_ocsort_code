#include <assert.h>

#include "default_roi_filterer.hpp"
#include "roi_utils.hpp"
#include "logger/logger.hpp"

DefaultROIFilterer::DefaultROIFilterer(std::shared_ptr<ROIDataHandler> data_handler)
    : IROIFilterer(data_handler){

}
DefaultROIFilterer::~DefaultROIFilterer(){

}

std::vector<vizpet::BoundingBox> DefaultROIFilterer::filter(std::vector<vizpet::BoundingBox> roi_boxes, cv::Mat image){
    // int width = image.cols;
    // int height = image.rows;
    //float small_criteria_multi = (max_object_count == 1) ? 0.1f : 0.3f;
    roi_boxes = remove_invalid_boxes(roi_boxes);
    //roi_boxes = ROIUtils::remove_smaller_boxes(roi_boxes, width, height, small_criteria_multi, max_object_count);
    //roi_boxes = ROIUtils::remove_bigger_boxes(roi_boxes, width, height);
    // roi_boxes  = remove_inner_contained_boxes(roi_boxes, image.cols, image.rows);
    //roi_boxes = ROIUtils::get_top_boxes(roi_boxes, width, height, max_object_count);
    return roi_boxes;
}

//removing boxes with priority = 0
std::vector<vizpet::BoundingBox> DefaultROIFilterer::remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0; i<boxes.size(); i++){
        int priority_index =1;
        // priority_index = roi_data_handler->get_priority(boxes[i].tag);
        bool is_valid = roi_data_handler->is_valid_id(boxes[i].tag_id);
        if(is_valid){
            filtered_boxes.push_back(boxes[i]);
        }
    }
    return filtered_boxes;
}
