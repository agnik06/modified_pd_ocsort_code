#ifndef __REFERENCE_POINT_ROI_FILTERER__
#define __REFERENCE_POINT_ROI_FILTERER__

#include "roi_filterer.hpp"

class ReferencePointROIFilterer: public IROIFilterer{
public:
    ReferencePointROIFilterer(std::shared_ptr<ROIDataHandler> data_handler);
    ~ReferencePointROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);

private:
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> get_box_near_reference_point(std::vector<vizpet::BoundingBox> boxes, vizpet::Point reference_point);
    
};

#endif //__REFERENCE_POINT_ROI_FILTERER__