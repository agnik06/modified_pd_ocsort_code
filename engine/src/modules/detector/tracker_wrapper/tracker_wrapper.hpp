#ifndef __TRACKER_INTERFACE__
#define __TRACKER_INTERFACE__

#include "framework/bounding_box.hpp"
#include "../mapper/data/roi_data_handler.hpp"
#include <opencv2/opencv.hpp>
#include "../../../core/logger/logger.hpp"
//#include "tracker/tracker.cpp"
// #include <opencv2/video/tracking.hpp>
// #include <opencv2/video/tracker.hpp>
#include <memory>


class TrackerWrapper{
    bool update(cv::Mat image,vizpet::Rectangle old_box,vizpet::Rectangle &new_box);
public:
    TrackerWrapper();
    ~TrackerWrapper();
    void set_tracker(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);
    
    std::vector<vizpet::BoundingBox> get_tracked_boxes( cv::Mat image);
    // void set_reference_frame(vizpet::Rectangle reference_frame);
    void set_max_objects(int n);

protected:
    std::vector<vizpet::BoundingBox> tracked_boxes;
    int max_object_count = 1;
    int frame_count=0;
    //Tracker tracker;
    
};

#endif //__TRACKER_INTERFACE__