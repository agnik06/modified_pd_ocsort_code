/**
 * @file   salient_od_tagger.hpp
 * @brief  SalientODTagger applies filters based on a set of predifined threshold
 *
 * This class provides for implemetation of SalientODTagger.
 */

#ifndef __SALIENT_OD_TAGGER__
#define __SALIENT_OD_TAGGER__

#include "tagger/base_tagger.hpp"

#include "modules/detector/net/detector_net.hpp"
#include "modules/detector/base_detector.hpp"
#include "modules/detector/net/roi_tensor_parser.hpp"
#include "modules/detector/mapper/data/roi_data_handler.hpp"
#include "modules/detector/mapper/default_roi_mapper.hpp"
#include "modules/detector/filterer/petface_over_person_roi_filterer.hpp"
#include "modules/detector/filterer/petface_over_person_roi_filterer_v2.hpp"
#include "tagger/hooks/bounding_box_hooks.hpp"
#include "model_config.hpp"
#include "modules/detector/mapper/data/roi_data_factory.hpp"


class SalientODTagger : public BaseTagger{
public:
    SalientODTagger(std::shared_ptr<DetectorNet> detector_net,model::Version version, std::vector<std::string> detector_layers){
        std::shared_ptr<ROIDataHandler> roi_data_handler = std::make_shared<ROIDataHandler>(ROIDataFactory::get_data(version));

        std::shared_ptr<IDetector> detector = std::make_shared<BaseDetector>(detector_net);
        /*
        V3: GhostDet output layer: "output"
        V2,V1: MobileDetDSP and MobilenetEdgeTPU output layer: "detection_out"
        */
        if (version == model::Version::V1 || version == model::Version::V2 || version == model::Version::V2_MULTI_FRAME){
            detector->set_detection_out_layer(detector_layers);
            detector->set_float_parser(get_2D_tensor_parser());
        }
        else{
            detector->set_detection_out_layer(detector_layers);
            detector->set_tensor_parser(get_ssd_tensor_parser());
        }
        set_detector(detector);

        auto roi_mapper = std::make_shared<DefaultROIMapper>(roi_data_handler);
        roi_mapper->setModelVersion(version);
        set_roi_mapper(roi_mapper);
        std::vector<std::string> nms_tags = {"petface","pets"};
        std::vector<float> thresholds = {0.80};
        //bounding_box_hooks.push_back(get_merge_category_boxes_hook(nms_tags,thresholds));
        //bounding_box_hooks.push_back(get_merge_pet_boxes_hook());
        //float moon_object_overlap_threshold = 0.7;
        // bounding_box_hooks.push_back(get_remove_object_moon_boxes_hook(moon_object_overlap_threshold));
        //bounding_box_hooks.push_back(get_remove_object_boxes_hook());
        // auto roi_filterer = std::make_shared<SelectiveThresholdROIFilterer>(roi_data_handler);
        std::shared_ptr<IROIFilterer> roi_filterer;
        if(version == model::Version::V2 || version == model::Version::V2_MULTI_FRAME){
            roi_filterer = std::make_shared<PetfaceOverPersonROIFiltererV2>(roi_data_handler,nms_tags,thresholds);
        }else{
            roi_filterer = std::make_shared<PetfaceOverPersonROIFilterer>(roi_data_handler,nms_tags,thresholds);
        }

        roi_filterer->setModelVersion(version);
        roi_filterer->set_max_objects(16);
        set_roi_filterer(roi_filterer);
 
    }
};

#endif //__SALIENT_OD_TAGGER__