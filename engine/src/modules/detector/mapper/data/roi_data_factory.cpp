#include "roi_data_factory.hpp"
#include "logger/logger.hpp"

ROIDataFactory::ROIDataFactory(){ }

json ROIDataFactory::get_data(model::Version version){
    switch(version){
        case model::Version::V1:
            #ifdef DUMPBOX
                LOGV("ROIDataFactory get_data() loading %d data", version);
            #endif
            return get_json_str_v1();
        case model::Version::V2:
            #ifdef DUMPBOX
                LOGV("ROIDataFactory get_data() loading %d data", version);
            #endif
          return get_json_str_v2();
        case model::Version::V2_MULTI_FRAME:
            #ifdef DUMPBOX
                LOGV("ROIDataFactory get_data() loading %d data", version);
            #endif
          return get_json_str_v2();
        
        default:
            LOGV("No data found for version: %d", version);
    }
    return NULL;
}

json ROIDataFactory::get_data(model::Version version, vizpet::PetDetector::Mode mode){
    /*if(mode == iudl::Mode::CH){
        return get_json_str_verifier();
    }*/
    return get_data(version);
}