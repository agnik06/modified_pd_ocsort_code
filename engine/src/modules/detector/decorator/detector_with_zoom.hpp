#ifndef __DETECTOR_WITH_ZOOM__
#define __DETECTOR_WITH_ZOOM__

#include "detector_decorator.hpp"

class DetectorWithZoom: public DetectorDecorator{
public:
    DetectorWithZoom(std::shared_ptr<IDetector> detector);
    ~DetectorWithZoom();
    std::vector<vizpet::BoundingBox> detect(cv::Mat image);
    float margin_scale;
};

#endif //__DETECTOR_WITH_ZOOM__