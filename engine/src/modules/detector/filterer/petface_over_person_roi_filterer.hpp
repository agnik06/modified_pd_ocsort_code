#ifndef __PETFACE_OVER_PERSON_ROI_FILTERER__
#define __PETFACE_OVER_PERSON_ROI_FILTERER__

#include "roi_filterer.hpp"

class PetfaceOverPersonROIFilterer: public IROIFilterer{
public:
    PetfaceOverPersonROIFilterer(std::shared_ptr<ROIDataHandler> data_handler,std::vector<std::string> &_overlap_check_categories,std::vector<float> _thresholds);
    ~PetfaceOverPersonROIFilterer();
    std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image);
    std::vector<std::string> overlap_check_categories;
    std::vector<float> thresholds;

private:
    std::vector<vizpet::BoundingBox> remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> remove_petface_over_person_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> ensure_petface_over_pet_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> ensure_petface_with_pet_boxes(std::vector<vizpet::BoundingBox> boxes);
    bool is_small_petface_boxes(vizpet::BoundingBox b1 ,  vizpet::BoundingBox b2);
    std::vector<vizpet::BoundingBox> remove_pet_within_pet_boxes(std::vector<vizpet::BoundingBox> boxes);
    std::vector<vizpet::BoundingBox> remove_multi_petface_with_in_pet_boxes(std::vector<vizpet::BoundingBox> boxes);
};

#endif //__PETFACE_OVER_PERSON_ROI_FILTERER__