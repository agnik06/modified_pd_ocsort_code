#include <assert.h>

#include "reference_point_roi_filterer.hpp"
#include "roi_utils.hpp"
#include "framework/point.hpp"
#include "logger/logger.hpp"

ReferencePointROIFilterer::ReferencePointROIFilterer(std::shared_ptr<ROIDataHandler> data_handler)
    : IROIFilterer(data_handler){
    // this->max_object_count = 100;
}
ReferencePointROIFilterer::~ReferencePointROIFilterer(){

}

std::vector<vizpet::BoundingBox> ReferencePointROIFilterer::filter(std::vector<vizpet::BoundingBox> roi_boxes, cv::Mat image){
    if(reference_frame == nullptr){
        LG<<"VZ Debug: Setting of reference frame is expected before filtering from ReferencePointROIFilterer";
        return {};
    }
    int width = image.cols;
    int height = image.rows;
    float small_criteria_multi = (max_object_count == 1) ? 0.1f : 0.05f;
    
    roi_boxes = remove_invalid_boxes(roi_boxes);
    roi_boxes = ROIUtils::remove_smaller_boxes(roi_boxes, width, height, small_criteria_multi, max_object_count);
    roi_boxes = ROIUtils::remove_bigger_boxes(roi_boxes, width, height);

    roi_boxes = get_box_near_reference_point(roi_boxes, reference_frame->get_center());
    roi_boxes = ROIUtils::get_top_boxes(roi_boxes, width, height, max_object_count);
    return roi_boxes;
}

//removing boxes with priority = 0
std::vector<vizpet::BoundingBox> ReferencePointROIFilterer::remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0; i<boxes.size(); i++){
        int priority_index = roi_data_handler->get_priority(boxes[i].tag);
        bool is_valid = roi_data_handler->is_valid_id(boxes[i].tag_id);
        if(priority_index > 0){
            filtered_boxes.push_back(boxes[i]);
        }
    }
    return filtered_boxes;
}


std::vector<vizpet::BoundingBox> ReferencePointROIFilterer::get_box_near_reference_point(std::vector<vizpet::BoundingBox> boxes, vizpet::Point reference_point){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(auto box: boxes){
        if(box.location.contains(reference_point)){
            filtered_boxes.push_back(box);
        }
    }
    if(filtered_boxes.empty()){
        std::sort(boxes.begin(), boxes.end(),
            [reference_point](vizpet::BoundingBox& box1, vizpet::BoundingBox& box2){
                int box1_score = pow(box1.location.get_center().x - reference_point.x, 2) +  pow(box1.location.get_center().y - reference_point.y, 2);
                int box2_score = pow(box2.location.get_center().x - reference_point.x, 2) +  pow(box2.location.get_center().y - reference_point.y, 2);
                return box1_score < box2_score;
            }
        );
        if(!boxes.empty())
            filtered_boxes.push_back(boxes[0]);
    }
    return filtered_boxes;
}
