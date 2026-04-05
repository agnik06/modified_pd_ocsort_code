#ifndef __DEFAULT_ROI_FILTERER__
#define __DEFAULT_ROI_FILTERER__

#include "roi_filterer.hpp"

class DefaultROIFilterer: public IROIFilterer{
public:
    DefaultROIFilterer(std::shared_ptr<ROIDataHandler> data_handler);
    ~DefaultROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);

private:
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
};

#endif //__DEFAULT_ROI_FILTERER__