#include "roi_filterer.hpp"

IROIFilterer::IROIFilterer(std::shared_ptr<ROIDataHandler> data_handler){
    this->roi_data_handler = data_handler;
}
IROIFilterer::~IROIFilterer(){

}

void IROIFilterer::set_max_objects(int n){
    this->max_object_count = n;
}

void IROIFilterer::set_reference_frame(vizpet::Rectangle reference_frame){
    this->reference_frame = std::make_shared<vizpet::Rectangle>(reference_frame);
}

void IROIFilterer::setModelVersion(model::Version version){
    #ifdef DUMPBOX
        LOGV("VZ Debug: roi_filter setModelVersion model version :%d", version);
    #endif
    this->modelVersion = version;
}

