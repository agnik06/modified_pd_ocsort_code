/**
 * @file   rectangle_utils.hpp
 * @brief  This file provides interfaces for different bounding box utilities
 *
 * This file provides interfaces for computing operations on bounding boxes detected
 */

#ifndef __RECTANGLE_UTILS__
#define __RECTANGLE_UTILS__

#include "framework/rectangle.hpp"
#include <vector>


#define IntVec std::vector<int>

class RectangleUtils{
    public:
        //Returns the index of the box from pre_boxes which has the maximum iou with box if the iou is greater than thresh otherwise returns -1.
        static IntVec iou_argmax_vector(std::vector<vizpet::Rectangle>& curr_boxes, std::vector<vizpet::Rectangle>& prev_boxes, float thresh=0.8);
        //computes IOU between 2 boxes based on threshold
        static int iou_argmax(vizpet::Rectangle box, std::vector<vizpet::Rectangle>& prev_boxes, float thresh);
        //compute the IOU between two rectangle boxes
        static float compute_iou(vizpet::Rectangle& box1, vizpet::Rectangle& box2);
        //computes the intersection between two rectangle boxes
        static int compute_intersection(vizpet::Rectangle& box1, vizpet::Rectangle& box2);
        //Computes the union of 2 rectangle boxes 
        static int compute_union(vizpet::Rectangle& box1, vizpet::Rectangle& box2, int intersection);
        //scales a rectangle box by scaling constant
        static vizpet::Rectangle scale_rectangle(vizpet::Rectangle box, float scaling_const);
        //Merge two rectangles to one
        static vizpet::Rectangle add_rectangles(vizpet::Rectangle box1, vizpet::Rectangle box2);
        //Area of Rectangle
        static int area_rectangle(vizpet::Rectangle box1);
};

#endif //__RECTANGLE_UTILS__