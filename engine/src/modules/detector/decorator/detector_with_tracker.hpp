#ifndef DETECTOR_WITH_TRACKER
#define DETECTOR_WITH_TRACKER

#include "detector_decorator.hpp"
#include "../tracker_wrapper/tracker_wrapper.hpp"
#include "../mapper/roi_mapper.hpp"
#include "../filterer/roi_filterer.hpp"

class DetectorWithTracker:public DetectorDecorator{

public:
    DetectorWithTracker(std::shared_ptr<IDetector> detector,
                        std::shared_ptr<IROIFilterer> roi_filter,
                        std::shared_ptr<IROIMapper> roi_mapper);
    TrackerWrapper tracker;
    std::vector<vizpet::BoundingBox> detect(cv::Mat image);
    std::shared_ptr<IROIMapper> roi_mapper;
    std::shared_ptr<IROIFilterer> roi_filterer;
    bool tracked;
    ~DetectorWithTracker();

};






#endif