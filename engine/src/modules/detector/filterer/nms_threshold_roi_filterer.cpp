#include <assert.h>

#include "nms_threshold_roi_filterer.hpp"
#include "roi_utils.hpp"
#include "logger/logger.hpp"

NmsThresholdROIFilterer::NmsThresholdROIFilterer(std::shared_ptr<ROIDataHandler> data_handler,std::vector<std::string> &_overlap_check_categories,std::vector<float> _thresholds)
    : IROIFilterer(data_handler){
     overlap_check_categories = _overlap_check_categories;
     thresholds = _thresholds;
}
NmsThresholdROIFilterer::~NmsThresholdROIFilterer(){

}

std::vector<vizpet::BoundingBox> NmsThresholdROIFilterer::filter(std::vector<vizpet::BoundingBox> roi_boxes, cv::Mat image){
    int width = image.cols;
    int height = image.rows;
    float small_criteria_multi = (max_object_count == 1) ? 0.1f : 0.3f;
    roi_boxes = remove_invalid_boxes(roi_boxes);
    roi_boxes = ROIUtils::remove_other_boxes_in_category_box(roi_boxes,overlap_check_categories,thresholds);
    roi_boxes=merge_category_boxes(roi_boxes);
    //roi_boxes = ROIUtils::remove_smaller_boxes(roi_boxes, width, height, small_criteria_multi, max_object_count);
    //roi_boxes = ROIUtils::remove_bigger_boxes(roi_boxes, width, height);
    // roi_boxes  = remove_inner_contained_boxes(roi_boxes, image.cols, image.rows);
    //roi_boxes = ROIUtils::get_top_boxes(roi_boxes, width, height, max_object_count);
    return roi_boxes;
}

std::vector<vizpet::BoundingBox> NmsThresholdROIFilterer::merge_category_boxes(std::vector<vizpet::BoundingBox> &boxes){
    std::vector<cv::Rect> picked_pet_boxes;
    std::vector<vizpet::BoundingBox> pet_boxes;
    vizpet::Size size;
    if(boxes.size()>0){
        size=boxes[0].image_size;
    }
    std::vector<vizpet::BoundingBox> picked;
    for(int i=0;i<boxes.size();i++){
        if(boxes[i].tag=="petface"){
            pet_boxes.push_back(boxes[i]);
        }
        else{
            picked.push_back(boxes[i]);
        }
    }
    
    for(int i=0;i<pet_boxes.size();i++){
        int is_not_merged=1;
        for(int j=i+1;j<pet_boxes.size();j++){
            float iou=ROIUtils::get_iou_between_boxes(pet_boxes[i].location,pet_boxes[j].location);
            
            if(iou>0.2){
                std::vector<cv::Rect> temp_pet_boxes;
                temp_pet_boxes.push_back(cv::Rect(pet_boxes[i].location.left,pet_boxes[i].location.top,pet_boxes[i].location.width(),pet_boxes[i].location.height()));
                temp_pet_boxes.push_back(cv::Rect(pet_boxes[j].location.left,pet_boxes[j].location.top,pet_boxes[j].location.width(),pet_boxes[j].location.height()));
                cv::groupRectangles(temp_pet_boxes,1,99);
                if(temp_pet_boxes.size()>0)
                picked_pet_boxes.push_back(temp_pet_boxes[0]);
                is_not_merged=0;
                pet_boxes.erase(pet_boxes.begin()+i);
                pet_boxes.erase(pet_boxes.begin()+j-1);
                i--;
                j-=2;
                break;
            }
        }
        if(is_not_merged)
            picked.push_back(pet_boxes[i]);
    }
    for(int i=0;i<picked_pet_boxes.size();i++){
        vizpet::Rectangle rect_=vizpet::Rectangle(picked_pet_boxes[i].x,
                                                        picked_pet_boxes[i].y,
                                                        picked_pet_boxes[i].x+picked_pet_boxes[i].width,
                                                        picked_pet_boxes[i].y+picked_pet_boxes[i].height);
        picked.push_back(vizpet::BoundingBox(rect_,size,"petface",0.50,8));
    }
    return picked;

}

//removing boxes with priority = 0
std::vector<vizpet::BoundingBox> NmsThresholdROIFilterer::remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes){
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
