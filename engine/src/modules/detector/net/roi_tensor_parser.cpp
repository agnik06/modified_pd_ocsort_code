/**
 * @file   roi_tensor_parser.cpp
 * @brief  This file provides interfaces for getting the parsed ssd tensor
 *
 * Implements for getting the parsed ssd tensor
 */
#include "roi_tensor_parser.hpp"

void correct_box_bounds(vizpet::Rectangle &obj_box, int &img_w, int &img_h){
    if (obj_box.left < 0)       obj_box.left = 0;
    if (obj_box.top < 0)        obj_box.top = 0;
    if (obj_box.right < 0)      obj_box.right = 0;
    if (obj_box.bottom < 0)     obj_box.bottom = 0;
    if (obj_box.left > img_w)   obj_box.left = img_w;
    if (obj_box.top > img_h)    obj_box.top = img_h;
    if (obj_box.right > img_w)  obj_box.right = img_w;
    if (obj_box.bottom > img_h) obj_box.bottom = img_h;
}

/**
 * @brief gets the ssd tensor parser
 * @details Scans all of n_boxes, corrects the boxes based on tensor 
 *          and the actual image width and height
 *
 * @param[in] none
 *
 * @return callback for ssd roi tensor with the function definition
 */
ROITensorParser get_ssd_tensor_parser(){
    ROITensorParser ssd_tensor_parser = [](Tensor3D<float> tensor, cv::Mat input_image){
        int n_boxes = tensor.H;
        int img_w = input_image.cols;
        int img_h = input_image.rows;

        std::vector<vizpet::BoundingBox> boxes;
        for (int i = 0; i < n_boxes; i++) {
            int id = (int)tensor(0, i, 1);
	    /* if id is zero ignore the box */
	    if(!id)
	        continue;
            vizpet::Rectangle obj_box;
            obj_box.left   = tensor(0, i, 3) * img_w;
            obj_box.top    = tensor(0, i, 4) * img_h;
            obj_box.right  = tensor(0, i, 5) * img_w;
            obj_box.bottom = tensor(0, i, 6) * img_h;
            correct_box_bounds(obj_box, img_w, img_h);

            float box_score  = tensor(0, i, 2);
            std::string label = "";

            vizpet::BoundingBox new_box(obj_box, {img_w, img_h}, label, box_score, id);
            boxes.push_back(new_box);
        }
        return boxes;
    };
    return ssd_tensor_parser;
}

ROIFloatParser get_2D_tensor_parser(){
    ROIFloatParser twoD_tensor_parser = [](std::vector<std::vector<float> > roi, cv::Mat input_image){
        int n_boxes = roi.size();
        int img_w = input_image.cols;
        int img_h = input_image.rows;
        // LOGV("get_2D_tensor_parser : Total boxes roi [%lu]", roi.size() );
        std::vector<vizpet::BoundingBox> boxes;
        for (int i = 0; i < n_boxes; i++) {
            int id = (int)roi[i][5];
            // LOGV("get_2D_tensor_parser : Total boxes id [%d]", id );
	    /* if id is zero ignore the box */
	    if(!id)
	        continue;
            vizpet::Rectangle obj_box;
            obj_box.left   = roi[i][0] * img_w;
            obj_box.top    = roi[i][1] * img_h;
            obj_box.right  = roi[i][2] * img_w;
            obj_box.bottom = roi[i][3] * img_h;
            correct_box_bounds(obj_box, img_w, img_h);

            float box_score  = roi[i][4];
            std::string label = std::to_string(roi[i][5]);

            vizpet::BoundingBox new_box(obj_box, {img_w, img_h}, label, box_score, id);
            boxes.push_back(new_box);
        }
        // LOGV("get_2D_tensor_parser : Total boxes [%lu]", boxes.size() );
        return boxes;
    };
    return twoD_tensor_parser;
}

