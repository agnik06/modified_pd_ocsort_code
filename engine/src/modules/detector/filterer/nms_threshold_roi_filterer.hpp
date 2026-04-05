#ifndef __NMS_THRESHOLD_ROI_FILTERER__
#define __NMS_THRESHOLD_ROI_FILTERER__

#include "roi_filterer.hpp"
#include "opencv2/objdetect.hpp"

class NmsThresholdROIFilterer: public IROIFilterer{
public:
    std::vector<std::string> overlap_check_categories;
    std::vector<float> thresholds;
    NmsThresholdROIFilterer(std::shared_ptr<ROIDataHandler> data_handler,std::vector<std::string> &_overlap_check_categories,std::vector<float> _thresholds);
    ~NmsThresholdROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);
    std::vector<vizpet::BoundingBox> merge_category_boxes(std::vector<vizpet::BoundingBox> &boxes);
private:
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
};

#endif //__SELECTIVE_THRESHOLD_ROI_FILTERER__