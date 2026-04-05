#ifndef __SELECTIVE_THRESHOLD_ROI_FILTERER__
#define __SELECTIVE_THRESHOLD_ROI_FILTERER__

#include "roi_filterer.hpp"

class SelectiveThresholdROIFilterer: public IROIFilterer{
public:
    SelectiveThresholdROIFilterer(std::shared_ptr<ROIDataHandler> data_handler);
    ~SelectiveThresholdROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);

private:
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
};

#endif //__SELECTIVE_THRESHOLD_ROI_FILTERER__