#ifndef __DETECTOR_WITH_TEMPORAL_SMOOTHING__
#define __DETECTOR_WITH_TEMPORAL_SMOOTHING__

#include "detector_decorator.hpp"
#include "utils/rectangle_utils.hpp"

#define Rect vizpet::Rectangle
#define RectVec std::vector<vizpet::Rectangle>
#define BoxVec std::vector<vizpet::BoundingBox>
#define IntVec std::vector<int>

class DetectorTempSmooth: public DetectorDecorator{
public:
    DetectorTempSmooth(std::shared_ptr<IDetector> detector);
    ~DetectorTempSmooth();
    std::vector<vizpet::BoundingBox> detect(cv::Mat image);

private:
    RectVec accelerations;
    RectVec prev_boxes;
    RectVec curr_boxes;
    bool first_frame;

    void handle_first_frame(BoxVec& image_rois);
    RectVec extract_rectangles(BoxVec& rois);
    void init_accelerations(int size);
    void handle_next_frame(BoxVec& image_rois);
    void reassign_accelerations(IntVec& argmax_vec, RectVec& accelerations);
    void smoothen_rectangles(IntVec& argmax_vec);
    RectVec second_order_smoothing(Rect& curr_box, Rect& prev_box, Rect& acceleration, float alpha=0.05, float beta=0.05);
    Rect box_op(Rect& box, Rect& prev_box, Rect& acceleration, float alpha);
    Rect accel_op(Rect& smoothed_box, Rect& prev_box, Rect& acceleration, float beta);
    BoxVec repopulate_boxes(BoxVec& rois, RectVec& rectangles);
};

#endif //__DETECTOR_WITH_TEMPORAL_SMOOTHING__