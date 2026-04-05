/**
 * @file   roi_data_handler.cpp
 * @brief  This file provides utilities for roi rectagle box
 *
 * This file provides utilities for roi rectagle box
 */

#include "roi_data_handler.hpp"
//#include "data_8_class.hpp"
#include "logger/logger.hpp"



ROIDataHandler::ROIDataHandler(){
    //json json_data = detector_json_str;
    //initialize(json_data);
}

ROIDataHandler::ROIDataHandler(json json_data){
    initialize(json_data);
}

ROIDataHandler::~ROIDataHandler(){
    
}

void ROIDataHandler::initialize(json json_str){
    json tags = json_str["roi_tags"];
    for (json ent : tags) {
        std::string label = ent["label"];
        int id = ent["id"];
        this->label_id_map[label]   = id;
        this->id_label_map[id]      = label;
        this->thresholds[label]     = ent["threshold"];
        this->type[label]           = ent["type"];
        this->cluster_label[label]  = ent.value("cluster_label", label);
        this->priority_index[label] = ent.value("priority_index", 0);
    }
}

std::string ROIDataHandler::get_label(int id){
    if(this->id_label_map.count(id) > 0){
        return this->id_label_map[id];
    }
    else{
        LG<<"VZ Debug: Label with id = "<<id<<" NOT FOUND";
    }
    return "";
}

int ROIDataHandler::get_priority(std::string label){
    if(this->priority_index.count(label) > 0){
        return this->priority_index[label];
    }
    else{
        LG<<"VZ Debug: Priority for label = "<<label<<" NOT FOUND";
    }
    return 0;
}

std::string ROIDataHandler::get_cluster_label(std::string label){
    if(this->cluster_label.count(label) > 0){
        return this->cluster_label[label];
    }
    else{
        LG<<"VZ Debug: clsuter for label = "<<label<<" NOT FOUND";
    }
    return "INVALID";
}


std::string ROIDataHandler::get_cluster_label(int id){
    std::string label=get_label(id);
    if(this->cluster_label.count(label) > 0){
        return get_cluster_label(label);
    }
    return "INVALID";
}

float ROIDataHandler::get_threshold(std::string label){
    return this->thresholds[label];
}

int ROIDataHandler::get_priority(int id){
    return get_priority(get_label(id));
}

bool ROIDataHandler::is_valid_id(int id){
    if(this->id_label_map.count(id) > 0){
        return true;
    }
    return false;
}