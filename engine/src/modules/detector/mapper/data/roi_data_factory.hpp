#ifndef __ROI_DATA_FACTORY__
#define __ROI_DATA_FACTORY__

#include "pet_detector.h"
#include "model_config.hpp"
#include "json.hpp"

using json = nlohmann::json;

json get_json_str_v1();
json get_json_str_v2();

class ROIDataFactory{
public:
    ROIDataFactory();
    static json get_data(model::Version version);
    static json get_data(model::Version version, vizpet::PetDetector::Mode mode);
};
#endif