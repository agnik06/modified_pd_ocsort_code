#include "detector_with_tracker.hpp"

#include "../filterer/roi_utils.hpp"




DetectorWithTracker::DetectorWithTracker(std::shared_ptr<IDetector> detector,
                                         std::shared_ptr<IROIFilterer> roi_filterer,
                                         std::shared_ptr<IROIMapper> roi_mapper)
                                         :DetectorDecorator(detector),tracked(0),roi_filterer(roi_filterer),roi_mapper(roi_mapper){
    tracker.set_max_objects(1);
} 


DetectorWithTracker::~DetectorWithTracker(){

}


std::vector<vizpet::BoundingBox> DetectorWithTracker::detect(cv::Mat image){
    std::vector<vizpet::BoundingBox> boxes;
    if(tracked){
        boxes=tracker.get_tracked_boxes(image);
        if(boxes.size()==0){
            tracked=0;
        }
    }
    if(!tracked){
        boxes = DetectorDecorator::detect(image);
        if(this->roi_mapper != nullptr){
            boxes = this->roi_mapper->map(boxes);
        }
        if(this->roi_filterer != nullptr){
            boxes = this->roi_filterer->filter(boxes, image);
        }
        if(boxes.size()!=0){
            tracker.set_tracker(boxes,image);
            tracked=1;
        }
    }
    
    return boxes;
}