/**
 * @file   tagger_factory.cpp
 * @brief  This file provides implementation for tagger factory
 *
 * This file provides implementation for tagger factory
 */

#include "tagger_factory.hpp"
#include "modules/detector/net/detector_net.hpp"
// #include "../data_handler/data/data_factory.hpp"

#include "salient_od_tagger.hpp"

TaggerFactory::TaggerFactory(){
}
TaggerFactory::~TaggerFactory(){
}

std::shared_ptr<BaseTagger> TaggerFactory::get_tagger(std::shared_ptr<Config> config, TaggerInitCallback tagger_init_cb){
    vizpet::PetDetector::Mode mode = config->getMode();
    std::string base_folder = config->getModelFilePath();
    model::Version version = config->getVersion();

    NNetInitCallback net_init_cb = NULL;
    NNetInitCallback multi_net_init_cb = NULL;
    if(tagger_init_cb != NULL){
        tagger_module_init_status.clear();
        tagger_module_init_status["aic_pet_detector"] = 0;

        multi_net_init_cb = [this, tagger_init_cb](bool status, std::string module_name){
            this->tagger_module_init_status[module_name] = status?1:-1; //1:Success -1:Fail 0:Not Started(default)
            bool tagger_init_status = true;
            bool tagger_init_done = true;
            for(std::map<std::string, int>::iterator it = tagger_module_init_status.begin(); it!= tagger_module_init_status.end(); it++){
                tagger_init_done    &= (it->second != 0);//all modules are not initialized
                tagger_init_status  &= (it->second == 1);//all modules are successfully initialized
            }
            LOGV("VZ Debug: %s module net initialization was %s", module_name.c_str(), status?"completed":"failed");
            if(tagger_init_done){
                tagger_init_cb(tagger_init_status);
            }
        };

        net_init_cb = [this, tagger_init_cb](bool status, std::string module_name){
            LOGV("VZ Debug: %s module net initialization was %s", module_name.c_str(), status?"completed":"failed");
            tagger_init_cb(status);
        };
    }

    if(mode == vizpet::PetDetector::Mode::SALIENCY || mode == vizpet::PetDetector::Mode::SALIENCY_SYSTEM){
            LOGV("VZ Debug: Mode - LOD Tagger_Factory Initialize mode %d",mode);

        std::vector<std::string> detector_layers;
        std::vector<std::string> input_layers;
        std::vector<float> means;
        /*
        V3: GhostDet output layer: "output"
        V2,V1: MobileDetDSP and MobilenetEdgeTPU output layer: "detection_out"
        */
        auto snap_config_detector = get_snap_config(config, "aic_pet_detector");
        int latency=std::get<2>(snap_config_detector);
        std::string detector_model_path   = get_model_path(base_folder, "aic_pet_detector", std::get<3>(snap_config_detector));

        if (version == model::Version::V1 || version == model::Version::V2) {
            LOGV("VZ Debug: get_tagger detection layer is output and means 128");
            detector_layers = {std::get<5>(snap_config_detector)};
            input_layers = {std::get<4>(snap_config_detector)};
            means = {128, 128, 128};
        }else if(version == model::Version::V2_MULTI_FRAME){
            LOGV("VZ Debug: get_tagger detection layer is output with multi frame and means 128");
            detector_layers = {std::get<5>(snap_config_detector), "out_feat_16", "out_feat_32", "out_feat_64", "out_feat_128"};
            input_layers = {std::get<4>(snap_config_detector), "feat_16", "feat_32", "feat_64", "feat_128"};
            means = {128, 128, 128};
        }
        else{
            LOGV("VZ Debug: get_tagger detection layer is detection_out and means 104, 117, 123");
            detector_layers = {std::get<5>(snap_config_detector), "out_feat_16", "out_feat_32", "out_feat_64", "out_feat_128"};
            input_layers = {std::get<4>(snap_config_detector), "feat_16", "feat_32", "feat_64", "feat_128"};
            means = {104, 117, 123};
        }

        
        std::shared_ptr<DetectorNet> detector_net = std::make_shared<DetectorNet>(
                                                        NNet::Builder() \
                                                        .set_model_path(detector_model_path) \
                                                        .set_module_name("aic_pet_detector") \
                                                        .set_output_layer_names(detector_layers) \
                                                        .set_compute_unit(std::get<0>(snap_config_detector)) \
                                                        .set_execution_data_type(std::get<1>(snap_config_detector)) \
                                                        .set_latency(latency) \
                                                        .set_input_layer_names(input_layers) \
                                                        .set_resolution(std::get<6>(snap_config_detector))
                                                        .set_init_callback_fn(net_init_cb) \
                                                        .set_channel_means(means) \
                                                        .validate(),std::get<6>(snap_config_detector),std::get<6>(snap_config_detector),version);

        switch(version){
            case model::Version::V1:
                    LOGV("VZ Debug: get_tagger laoding the version %d tagger", version);
                return std::make_shared<SalientODTagger>(detector_net,version,detector_layers);
            case model::Version::V2:
                    LOGV("VZ Debug: get_tagger laoding the version %d tagger", version);
                return std::make_shared<SalientODTagger>(detector_net,version,detector_layers);
            case model::Version::V2_MULTI_FRAME:
                    LOGV("VZ Debug: get_tagger laoding the version %d tagger", version);
                return std::make_shared<SalientODTagger>(detector_net,version,detector_layers);

        }

    }
    return nullptr;
}

std::string TaggerFactory::get_model_path(std::string base_folder, std::string module_name, std::string model_path){
    if(model_path.empty()){
        return base_folder;
    }
    return model_path;
}

std::tuple<int, int, int, std::string, std::string, std::string, int> TaggerFactory::get_snap_config(std::shared_ptr<Config> config, std::string module_name){
    int compute_unit = static_cast<int>(snap::ComputeUnit::CPU);
    int execution_type = static_cast<int>(snap::ExecutionDataType::FLOAT16);
    int latency = 11;
    int resolution = 512;
    std::string input_layer = "data";
    std::string output_layer = "output";
    std::string model_path = "";


    for(ModuleProperty module: config->getModules()){
        if(module.name == module_name){
            compute_unit = static_cast<int>(module.compute_unit);
            execution_type = static_cast<int>(module.execution_type);
            model_path = module.model_path;
            latency=module.latency;
            resolution = module.resolution;
            if(module.compilier == "MTK"){
                resolution = 512;
                input_layer = "data";
                output_layer = "output";
            }
            else if(compute_unit == 1 || compute_unit == 0){
                resolution = 512;
                input_layer = "inputs_0";
                output_layer = "Identity";
            }
            break;
        }
    }
    LOGV("TaggerFactory::get_snap_config resolution : [%d]", resolution);
    return std::make_tuple(compute_unit, execution_type, latency, model_path, input_layer, output_layer, resolution);
}
