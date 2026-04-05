/**
 * @file   bounding_box_hooks.cpp
 * @brief  This file provides interfaces for hooks
 *
 * This interfaces for the hooks are defined based on the custom need of entity
 */

#include "bounding_box_hooks.hpp"

#include "logger/logger.hpp"

/**
 * @brief This hook mergers the boxes based on the categories
 * @details Merging criteria based on category and thresholds
 *
 * @param[in] takes caregories and thresholds
 *
 * @return bool status
 */
BoundingBoxGeneratorHook get_merge_category_boxes_hook(std::vector<std::string> &category,std::vector<float> &thresholds){
    BoundingBoxGeneratorHook hook = [category,thresholds](std::vector<vizpet::BoundingBox> boxes, std::vector<vizpet::Tag> tags){
        //return boxes; //presently disabling the hook logic and needs revisiting since its causing box jumps
    std::vector<cv::Rect> picked_pet_boxes;
    std::vector<vizpet::BoundingBox> pet_boxes;
    vizpet::Size size;
    if(boxes.size()>0){
        size=boxes[0].image_size;
    }
    std::vector<vizpet::BoundingBox> picked;
    for(int i=0;i<boxes.size();i++){
        if(boxes[i].tag=="pets"){
            pet_boxes.push_back(boxes[i]);
        }
        else{
            picked.push_back(boxes[i]);
        }
    }
    
    for(int i=0;i<pet_boxes.size();i++){
        int is_not_merged=1;
        for(int j=i+1;j<pet_boxes.size();j++){
            float iou = RectangleUtils::compute_iou(pet_boxes[i].location,pet_boxes[j].location);
            #ifdef DUMPBOX
                LOGV("VZ Debug: get_merge_category_boxes_hook compute_iou: [%f]", iou);
            #endif

            if(iou>0.85){
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
        picked.push_back(vizpet::BoundingBox(rect_,size,"pets",0.70,5));
    }
    #if DUMPBOX
        LOGV("VZ Debug: get_merge_category_boxes_hook No. of boxes original: [%d] Final : [%d]", boxes.size(), picked.size());
    #endif
    return picked;
    };
    return hook;
}

/**
 * @brief This hook mergers the boxes based on the categories
 * @details Merging criteria based on category and thresholds
 *
 * @param[in] takes caregories and thresholds
 *
 * @return bool status
 */
BoundingBoxGeneratorHook get_merge_pet_boxes_hook(){
    BoundingBoxGeneratorHook hook = [](std::vector<vizpet::BoundingBox> boxes, std::vector<vizpet::Tag> tags){
        std::vector<cv::Rect> picked_pet_boxes;
    std::vector<vizpet::BoundingBox> pet_boxes;
    vizpet::Size size;
    if(boxes.size()>0){
        size=boxes[0].image_size;
    }
    std::vector<vizpet::BoundingBox> picked;
    for(int i=0;i<boxes.size();i++){
        if(boxes[i].tag=="pets"){
            pet_boxes.push_back(boxes[i]);
        }
        else{
            picked.push_back(boxes[i]);
        }
    }
    
    for(int i=0;i<pet_boxes.size();i++){
        int is_not_merged=1;
        for(int j=i+1;j<pet_boxes.size();j++){
            float iou=RectangleUtils::compute_iou(pet_boxes[i].location,pet_boxes[j].location);
            
            if(iou>0.16){
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
        picked.push_back(vizpet::BoundingBox(rect_,size,"pets",0.78,5));
        #ifdef DUMPBOX
            LOGV("VZ Debug: get_merge_category_boxes_hook pets");
        #endif
    }
    return picked;
    };
    return hook;
}

/**
 * @brief This hook removes the moon object boxes
 * @details Merging criteria based on category and thresholds
 *
 * @param[in] takes caregories and thresholds
 *
 * @return bool status
 */
BoundingBoxGeneratorHook get_remove_object_moon_boxes_hook(float moon_overlap_threshold){
    BoundingBoxGeneratorHook hook = [moon_overlap_threshold](std::vector<vizpet::BoundingBox> boxes,std::vector<vizpet::Tag> tags){
        std::vector<vizpet::BoundingBox> picked;
        for(int i=0;i<boxes.size();i++){
            if(boxes[i].tag_id == 9){
                int is_overlapping = 0;
                for(int j = 0;j < boxes.size(); j++){
                    if(boxes[j].tag_id == 1 ){
                        float iou =RectangleUtils::compute_iou(boxes[i].location,boxes[j].location);
                        if(iou > moon_overlap_threshold){
                            is_overlapping = 1;
                        }
                    }
                }
                if(is_overlapping == 0){
                    picked.push_back(boxes[i]);
                }
            }
            else if(boxes[i].tag_id != 1 ){
                picked.push_back(boxes[i]);
            }
            else{
                continue;
            }
        }
        return picked;
    };
    return hook;
}

/**
 * @brief This hook removes the boxes based on the categories
 * @details Merging criteria based on category and thresholds
 *
 * @param[in] takes caregories and thresholds
 *
 * @return bool status
 */
BoundingBoxGeneratorHook get_remove_object_boxes_hook(){
    BoundingBoxGeneratorHook hook = [](std::vector<vizpet::BoundingBox> boxes,std::vector<vizpet::Tag> tags){
        std::vector<vizpet::BoundingBox> picked;
        for(int i=0;i<boxes.size();i++){
            if(boxes[i].tag_id != 1 ){
                picked.push_back(boxes[i]);
            }
            else{
                continue;
            }
        }
        return picked;
    };
    return hook;
}
