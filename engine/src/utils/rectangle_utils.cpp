/**
 * @file   rectangle_utils.cpp
 * @brief  This file implements different utilities for ROI box
 *
 * Different utilities are implemented for ROI box
 */
#include "rectangle_utils.hpp"

/**
 * @brief Returns the index of the box from pre_boxes which has the maximum 
 *        iou with box if the iou is greater than thresh otherwise returns -1
 * 
 * @param[in] curr_boxes represents all of boxes
 * @param[in] prev_boxes represents the prev_boxes
 * @param[in] thresh represets the thresholds of the boxes
 *
 * @return vector of boxes
 */
IntVec RectangleUtils::iou_argmax_vector(std::vector<vizpet::Rectangle>& curr_boxes, std::vector<vizpet::Rectangle>& prev_boxes, float thresh){
    IntVec argmax_vec;
    for(auto const& curr_box: curr_boxes)
        argmax_vec.push_back(iou_argmax(curr_box, prev_boxes, thresh));
    return argmax_vec;
}

/**
 * @brief Returns the index of the box from pre_boxes which has the maximum 
 *        iou with box if the iou is greater than thresh otherwise returns -1
 * 
 * @param[in] curr_boxes represents all of boxes
 * @param[in] prev_boxes represents the prev_boxes
 * @param[in] thresh represets the thresholds of the boxes
 *
 * @return vector of boxes
 */
int RectangleUtils::iou_argmax(vizpet::Rectangle box, std::vector<vizpet::Rectangle>& prev_boxes, float thresh){
    int max_i = 0;
    float max_iou = 0;
    for(int i = 0; i < prev_boxes.size(); i++){
        float iou = compute_iou(box, prev_boxes[i]);
        if (iou > max_iou){
            max_i = i;
            max_iou = iou;
        }
    }

    if (max_iou < thresh)
        max_i = -1;
    return max_i;
}

/**
 * @brief compute_iou computes the IOU between two rectangle boxes
 * 
 * @param[in] box1, box2 refers to boxes between which iou  is calculated
 *
 * @return float representing the iou value
 */
float RectangleUtils::compute_iou(vizpet::Rectangle& box1, vizpet::Rectangle& box2){
    int intersection = compute_intersection(box1, box2);
    int _union = compute_union(box1, box2, intersection);
    return (float) intersection / _union;
}

/**
 * @brief compute_intersection computes the intersection between two rectangle boxes
 * 
 * @param[in] box1, box2 refers to boxes between which intersection is calculated
 *
 * @return int representing the area of intersection
 */
int RectangleUtils::compute_intersection(vizpet::Rectangle& box1, vizpet::Rectangle& box2){
    int left = std::max(box1.left, box2.left);
    int top = std::max(box1.top, box2.top);
    int right = std::min(box1.right, box2.right);
    int bottom = std::min(box1.bottom, box2.bottom);
    return vizpet::Rectangle(left, top, right, bottom).area();
}

/**
 * @brief compute_union computes the union between the 2 boxes
 * 
 * @param[in] box1, box2 refers to boxes between which union is calculated
 * @param[in] intersection represents intersection area
 *
 * @return int represents the area of  union between the 2 boxes
 */
int RectangleUtils::compute_union(vizpet::Rectangle& box1, vizpet::Rectangle& box2, int intersection){
    int area1 = box1.area();
    int area2 = box2.area();
    return area1 + area2 - intersection;
}

/**
 * @brief scale_rectangle scales an ROI by scaling constant
 * 
 * @param[in] box represents the ROI which needs scaling
 * @param[in] scaling_const represents the value with which scaling needs to be done
 *
 * @return int represents the area of  union between the 2 boxes
 */
vizpet::Rectangle RectangleUtils::scale_rectangle(vizpet::Rectangle box, float scaling_const){
    int left = box.left * scaling_const;
    int top = box.top * scaling_const;
    int right = box.right * scaling_const;
    int bottom = box.bottom * scaling_const;
    return vizpet::Rectangle(left, top, right, bottom);
}

/**
 * @brief add_rectangles adds 2 rectangle boxes
 * 
 * @param[in] box represents the ROI which needs scaling
 * @param[in] scaling_const represents the value with which scaling needs to be done
 *
 * @return int represents the area of  union between the 2 boxes
 */
vizpet::Rectangle RectangleUtils::add_rectangles(vizpet::Rectangle box1, vizpet::Rectangle box2){
    int left = box1.left + box2.left;
    int top = box1.top + box2.top;
    int right = box1.right + box2.right;
    int bottom = box1.bottom + box2.bottom;
    return vizpet::Rectangle(left, top, right, bottom);
}

/**
 * @brief Area of the rectangle
 * 
 * @param[in] box1 represents the ROI which needs Area
 *
 * @return int represents the area of rectangle
 */
int RectangleUtils::area_rectangle(vizpet::Rectangle box1){
    int width = box1.right-box1.left+1;
    int height = box1.bottom-box1.top+1;
    return width * height;
}
