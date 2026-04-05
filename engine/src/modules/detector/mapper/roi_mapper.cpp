/**
 * @file   roi_mapper.cpp
 * @brief  This file implemets the ROI Mapper
 *
 * This file implemets the ROI Mapper
 */

#include "roi_mapper.hpp"

IROIMapper::IROIMapper(std::shared_ptr<ROIDataHandler> data_handler){
    this->roi_data_handler = data_handler;
}
IROIMapper::~IROIMapper(){

}

void IROIMapper::setModelVersion(model::Version version){
    #ifdef DUMPBOX
        LOGV("VZ Debug: roi_mapper setModelVersion model version :%d", version);
    #endif
    this->modelVersion = version;
}