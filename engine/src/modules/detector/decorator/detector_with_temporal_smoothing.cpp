#include "detector_with_temporal_smoothing.hpp"
#include "../filterer/roi_utils.hpp"

DetectorTempSmooth::DetectorTempSmooth(std::shared_ptr<IDetector> detector):DetectorDecorator(detector),first_frame(true){

}

DetectorTempSmooth::~DetectorTempSmooth(){

}

BoxVec DetectorTempSmooth::detect(cv::Mat image){
    BoxVec image_rois = DetectorDecorator::detect(image);
    if (first_frame){
        handle_first_frame(image_rois);
        return image_rois;
    }else{
        handle_next_frame(image_rois);
        return repopulate_boxes(image_rois, prev_boxes);
    }
}

void DetectorTempSmooth::handle_first_frame(BoxVec& image_rois){
    prev_boxes = extract_rectangles(image_rois);
    init_accelerations(prev_boxes.size());
    curr_boxes = prev_boxes;
    first_frame = false;
}

RectVec DetectorTempSmooth::extract_rectangles(BoxVec& rois){
    RectVec rectangles;
    for(auto const& roi: rois)
        rectangles.push_back(roi.location);
    return rectangles;
}

void DetectorTempSmooth::init_accelerations(int size){
    for(int i = 0; i < size; i++)
        accelerations.push_back(Rect(0, 0, 0, 0));
}

void DetectorTempSmooth::handle_next_frame(BoxVec& image_rois){
    curr_boxes = extract_rectangles(image_rois);
    IntVec argmax_vec = RectangleUtils::iou_argmax_vector(curr_boxes, prev_boxes);
    reassign_accelerations(argmax_vec, accelerations);
    smoothen_rectangles(argmax_vec);        // results updated in prev_boxes
}

void DetectorTempSmooth::reassign_accelerations(IntVec& argmax_vec, RectVec& accelerations){
    RectVec new_accelerations;
    for(auto const& idx: argmax_vec)
        if(idx == -1)
            new_accelerations.push_back(Rect(0, 0, 0, 0));
        else
            new_accelerations.push_back(accelerations[idx]);
    accelerations = new_accelerations;
}

void DetectorTempSmooth::smoothen_rectangles(IntVec& argmax_vec){
    for(int i = 0; i < argmax_vec.size(); i++){
        int idx = argmax_vec[i];
        if(idx!=-1){
            RectVec results = second_order_smoothing(curr_boxes[i], prev_boxes[idx], accelerations[i]);
            curr_boxes[i] = results[0];
            accelerations[i] = results[1];
        }
    }
    prev_boxes = curr_boxes;
}

RectVec DetectorTempSmooth::second_order_smoothing(Rect& curr_box, Rect& prev_box, Rect& acceleration, float alpha, float beta){
    RectVec results;
    Rect smoothened_box = box_op(curr_box, prev_box, acceleration, alpha);
    Rect smoothened_acceleration = accel_op(smoothened_box, prev_box, acceleration, beta);
    results.push_back(smoothened_box);
    results.push_back(smoothened_acceleration);
    return results;
}

Rect DetectorTempSmooth::box_op(Rect& box, Rect& prev_box, Rect& acceleration, float alpha){
    Rect scaled_box = RectangleUtils::scale_rectangle(box, alpha);
    Rect changed_prev_box = RectangleUtils::add_rectangles(prev_box, acceleration);
    Rect scaled_prev_box = RectangleUtils::scale_rectangle(changed_prev_box, 1.0-alpha);
    return RectangleUtils::add_rectangles(scaled_box, scaled_prev_box);
}

Rect DetectorTempSmooth::accel_op(Rect& smoothened_box, Rect& prev_box, Rect& acceleration, float beta){
    Rect diff = RectangleUtils::add_rectangles(smoothened_box, RectangleUtils::scale_rectangle(prev_box, -1));
    Rect scaled_diff = RectangleUtils::scale_rectangle(diff, beta);
    Rect scaled_acceleration = RectangleUtils::scale_rectangle(acceleration, 1 - beta);
    return RectangleUtils::add_rectangles(scaled_diff, scaled_acceleration);
}

BoxVec DetectorTempSmooth::repopulate_boxes(BoxVec& rois, RectVec& rectangles){
    for(int i = 0; i < rois.size(); i++)
        rois[i].location = rectangles[i];
    return rois;
}
