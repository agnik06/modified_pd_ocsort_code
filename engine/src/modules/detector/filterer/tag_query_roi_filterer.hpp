#ifndef __TAG_QUERY_ROI_FILTERER__
#define __TAG_QUERY_ROI_FILTERER__

#include "roi_filterer.hpp"

class TagQueryROIFilterer: public IROIFilterer{
public:
    TagQueryROIFilterer(std::shared_ptr<ROIDataHandler> data_handler, std::vector<std::string> tags);
    ~TagQueryROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);

private:
    std::vector<std::string> query_tags;
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> filter_queried_boxes(std::vector<vizpet::BoundingBox> boxes, std::vector<std::string> query_tags);
};

#endif //__TAG_QUERY_ROI_FILTERER__