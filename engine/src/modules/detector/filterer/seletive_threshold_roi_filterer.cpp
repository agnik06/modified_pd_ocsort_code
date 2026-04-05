#include <assert.h>

#include "seletive_threshold_roi_filterer.hpp"
#include "roi_utils.hpp"
#include "logger/logger.hpp"

SelectiveThresholdROIFilterer::SelectiveThresholdROIFilterer(std::shared_ptr<ROIDataHandler> data_handler)
    : IROIFilterer(data_handler){

}
SelectiveThresholdROIFilterer::~SelectiveThresholdROIFilterer(){

}

std::vector<vizpet::BoundingBox> SelectiveThresholdROIFilterer::filter(std::vector<vizpet::BoundingBox> roi_boxes, cv::Mat image){
    int width = image.cols;
    int height = image.rows;
    float small_criteria_multi = (max_object_count == 1) ? 0.1f : 0.3f;
    roi_boxes = remove_invalid_boxes(roi_boxes);
    //roi_boxes = ROIUtils::remove_smaller_boxes(roi_boxes, width, height, small_criteria_multi, max_object_count);
    //roi_boxes = ROIUtils::remove_bigger_boxes(roi_boxes, width, height);
    // roi_boxes  = remove_inner_contained_boxes(roi_boxes, image.cols, image.rows);
    //roi_boxes = ROIUtils::get_top_boxes(roi_boxes, width, height, max_object_count);
    return roi_boxes;
}

//removing boxes with priority = 0
std::vector<vizpet::BoundingBox> SelectiveThresholdROIFilterer::remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0; i<boxes.size(); i++){
        int priority_index =1;
        // priority_index = roi_data_handler->get_priority(boxes[i].tag);
        bool is_valid = roi_data_handler->is_valid_id(boxes[i].tag_id);
        if((priority_index > 0)&&(is_valid)){
            //applying thresholds
            if(roi_data_handler->get_threshold(roi_data_handler->get_label(boxes[i].tag_id))<boxes[i].score){
                filtered_boxes.push_back(boxes[i]);
            }
        }
    }
    return filtered_boxes;
}
