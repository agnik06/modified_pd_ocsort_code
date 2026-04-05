
#include <assert.h>

#include "petface_over_person_roi_filterer.hpp"
#include "roi_utils.hpp"
#include "logger/logger.hpp"

PetfaceOverPersonROIFilterer::PetfaceOverPersonROIFilterer(std::shared_ptr<ROIDataHandler> data_handler,std::vector<std::string> &_overlap_check_categories,std::vector<float> _thresholds)
    : IROIFilterer(data_handler){
        overlap_check_categories = _overlap_check_categories;
        thresholds = _thresholds;

}
PetfaceOverPersonROIFilterer::~PetfaceOverPersonROIFilterer(){

}

std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::filter(std::vector<vizpet::BoundingBox> roi_boxes, cv::Mat image){
    int width = image.cols;
    int height = image.rows;
    float small_criteria_multi = (max_object_count == 1) ? 0.1f : 0.3f;
    #if DUMPBOX
        LOGV("VZ Debug: Pre filter  No. of boxes detected: [%d]", roi_boxes.size());
    #endif
    #if DUMPBOX
        LOGV("VZ Debug: before Filter No. of boxes : [%d]",roi_boxes.size());
        for(int i =0;i<roi_boxes.size();i++){
            //if(roi_boxes[i].tag_id == 5){
                LOGV("VZ Debug: before Filter tag id : [%d], tag class : [%s] ",roi_boxes[i].tag_id,roi_boxes[i].tag.c_str());
            //}
        }
    #endif
    roi_boxes = remove_invalid_boxes(roi_boxes);
    #if DUMPBOX
        LOGV("VZ Debug: remove_invalid_boxes No. of boxes : [%d]",roi_boxes.size());
    #endif
#if 0
    roi_boxes = ROIUtils::remove_other_boxes_in_category_box(roi_boxes,overlap_check_categories,thresholds);
    #if DUMPBOX
        LOGV("VZ Debug: remove_other_boxes_in_category_box No. of boxes : [%d]",roi_boxes.size());
    #endif
#endif
    //roi_boxes = ROIUtils::remove_smaller_boxes(roi_boxes, width, height, small_criteria_multi, max_object_count);
    #if DUMPBOX
        LOGV("VZ Debug: remove_smaller_boxes No. of boxes : [%d]",roi_boxes.size());
    #endif
    //roi_boxes = ROIUtils::remove_bigger_boxes(roi_boxes, width, height);
    #if DUMPBOX
        LOGV("VZ Debug: remove_bigger_boxes No. of boxes : [%d]",roi_boxes.size());
    #endif
    //LOGV("VZ Debug: PetfaceOverPersonROIFilterer Enablinng small box removal for flower and animal");
    roi_boxes = ROIUtils::remove_smaller_boxes_for_Specific_categories(roi_boxes, width, height, max_object_count);
    #if DUMPBOX
        LOGV("VZ Debug: remove_smaller_boxes_for_Specific_categories No. of boxes : [%d]",roi_boxes.size());
    #endif
#if 0
    //unused for cats and dogs, todo when person class is considered, the revisit this logic
    roi_boxes  = remove_petface_over_person_boxes(roi_boxes);
    #if DUMPBOX1
        LOGV("VZ Debug: remove_petface_over_person_boxes No. of boxes : [%d]",roi_boxes.size());
        for(int i =0;i<roi_boxes.size();i++){
            //if(roi_boxes[i].tag_id == 5){
                LOGV("VZ Debug: remove_petface_over_person_boxes tag id : [%d], tag class : [%s] ",roi_boxes[i].tag_id,roi_boxes[i].tag.c_str());
            //}
        }
    #endif
#endif
    roi_boxes  = ensure_petface_over_pet_boxes(roi_boxes);
    #if DUMPBOX
        LOGV("VZ Debug: ensure_petface_over_pet_boxes No. of boxes : [%d]",roi_boxes.size());
    #endif
    roi_boxes  = remove_pet_within_pet_boxes(roi_boxes);
    #if DUMPBOX
        LOGV("VZ Debug: remove_pet_within_pet_boxes after No. of boxes : [%d]",roi_boxes.size());
    #endif
    /*roi_boxes  = remove_multi_petface_with_in_pet_boxes(roi_boxes);
    #if DUMPBOX
        LOGV("VZ Debug: remove_multi_petface_with_in_pet_boxes after No. of boxes : [%d]",roi_boxes.size());
    #endif*/
    //roi_boxes  = generate_association_ids(roi_boxes);
    //roi_boxes = ROIUtils::get_top_boxes(roi_boxes, width, height, max_object_count);
    return roi_boxes;
}

std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::remove_petface_over_person_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0;i<boxes.size();i++){
        if(roi_data_handler->get_label(boxes[i].tag_id)!="petface"){
            filtered_boxes.push_back(boxes[i]);
            continue;
        }
        int flag=0;
        for(int j=0;j<boxes.size();j++){
            if(roi_data_handler->get_label(boxes[j].tag_id) == "person"){
                int overlap_person = ROIUtils::check_overlap_box(boxes[j].location,boxes[i].location,0.8);
                if(overlap_person == 1){
                    flag=1;
                    for(int k = 0;k < boxes.size();k++){
                        if(roi_data_handler->get_label(boxes[k].tag_id) == "pets"){
                            int overlap_animal = ROIUtils::check_overlap_box(boxes[k].location,boxes[i].location,0.8);
                            if(overlap_animal == 1){
                                filtered_boxes.push_back(boxes[i]);
                            }
                        }
                    }
                }
            }
        }
        if(flag==0){
           filtered_boxes.push_back(boxes[i]); 
        }
    }
    return filtered_boxes;
}

std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::remove_pet_within_pet_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0;i<boxes.size();i++){
        if(roi_data_handler->get_label(boxes[i].tag_id)!="pets"){
            filtered_boxes.push_back(boxes[i]);
            continue;
        }
    }
    std::vector <int> over_lap_pet_index;
    if(!over_lap_pet_index.size()){
        over_lap_pet_index.clear();
    }
    for(int j=0;j<boxes.size();j++){
        if(roi_data_handler->get_label(boxes[j].tag_id) == "pets"){
            int overlap_pet = -1;
            for(int k = j+1;k < boxes.size();k++){
                if(roi_data_handler->get_label(boxes[k].tag_id) == "pets" && j != k){
                    overlap_pet = ROIUtils::check_overlap_box(boxes[k].location,boxes[j].location,0.8);
                    if(overlap_pet == 0){
                        over_lap_pet_index.push_back(k);
                    }else if(overlap_pet == 1){
                        over_lap_pet_index.push_back(j);
                    }
                }
            }
        }
    }
    //if(!over_lap_pet_index.size()){
        for(int i =0;i<boxes.size();i++){
            if(roi_data_handler->get_label(boxes[i].tag_id)=="pets" ){
                if(std::find(over_lap_pet_index.begin(),over_lap_pet_index.end(),i) == over_lap_pet_index.end()){
                    filtered_boxes.push_back(boxes[i]);
                }
            }
        }
        //}
    return filtered_boxes;
}   


std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::ensure_petface_over_pet_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int j=0;j<boxes.size();j++){
        if(roi_data_handler->get_label(boxes[j].tag_id)!="pets"){
            filtered_boxes.push_back(boxes[j]);
            continue;
        }
        if(roi_data_handler->get_label(boxes[j].tag_id) == "pets"){
            int added = 0;
            for(int k = 0;k < boxes.size(); k++){
                if(roi_data_handler->get_label(boxes[k].tag_id) == "petface"){
                    int overlap_pet = 0;
                    float overlap = ROIUtils::get_overlap_between_boxes(boxes[j].location,boxes[k].location);
                    #if DUMPBOX
                        LOGV("VZ Debug: ensure_petface_over_pet_boxes adding overlap=[%f]",overlap);
                    #endif
                    if(overlap > 0.8)
                        overlap_pet = 1;

                    if(overlap_pet == 1){
                        #if DUMPBOX
                            LOGV("VZ Debug: ensure_petface_over_pet_boxes adding id=[%d], threshold: [%f]",boxes[j].tag_id, boxes[j].score);
                        #endif
                        filtered_boxes.push_back(boxes[j]);
                        added = 1;
                        break;
                    }
                }
            }
            if(!added){
                if(boxes[j].score > 0.6){
                    #if DUMPBOX
                        LOGV("VZ Debug: ensure_petface_over_pet_boxes with no petfacebox adding id=[%d], threshold: [%f]",boxes[j].tag_id, boxes[j].score);
                    #endif
                    filtered_boxes.push_back(boxes[j]);
                } else {
                    #if DUMPBOX
                        LOGV("VZ Debug: ensure_petface_over_pet_boxes Skipping adding id=[%d], threshold: [%f]",boxes[j].tag_id, boxes[j].score);
                    #endif
                }
            }
        }
    }
    return filtered_boxes;
}

//removing boxes with priority = 0
std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::remove_invalid_boxes(std::vector<vizpet::BoundingBox> boxes){
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i=0; i<boxes.size(); i++){
        int priority_index =0;
        priority_index = roi_data_handler->get_priority(boxes[i].tag);
        bool is_valid = roi_data_handler->is_valid_id(boxes[i].tag_id);
        if((priority_index > 0)&&(is_valid)){
            //applying thresholds
            if(roi_data_handler->get_threshold(roi_data_handler->get_label(boxes[i].tag_id))<boxes[i].score){
                //LOGV("VZ Debug: remove_invalid_boxes adding id=[%d], threshold: [%f]",boxes[i].tag_id, boxes[i].score);
                filtered_boxes.push_back(boxes[i]);
            }
            /*else{
                LOGV("VZ Debug: remove_invalid_boxes error id=[%d], threshold: [%f]",boxes[i].tag_id, boxes[i].score);
            }*/
        }
    }
    return filtered_boxes;
}

std::vector<vizpet::BoundingBox> PetfaceOverPersonROIFilterer::remove_multi_petface_with_in_pet_boxes(std::vector<vizpet::BoundingBox> boxes){
   #ifdef DUMPBOX
    LOGV("remove_multi_petface_with_in_pet_boxes");
   #endif
    std::vector<vizpet::BoundingBox> filtered_boxes;
    for(int i = 0;i < boxes.size();i++){
        if(roi_data_handler->get_label(boxes[i].tag_id)!="petface"){
            filtered_boxes.push_back(boxes[i]);
        }
    }
    for(int j=0;j<boxes.size();j++){
        if(roi_data_handler->get_label(boxes[j].tag_id) == "pets"){
            float max_petface_threshold = 0.0f;
            int max_petface_threshold_index = -1;
            int overlap_pet = -1;
            for(int k = 0;k < boxes.size(); k++){
                if(roi_data_handler->get_label(boxes[k].tag_id) == "petface"){
                    overlap_pet = ROIUtils::check_overlap_box(boxes[j].location,boxes[k].location,0.80);
                    //LOGV("remove_multi_petface_with_in_pet_boxes overlap_pet : [%d], max_petface_threshold : [%f], max_thereshold_petface_box.left : [%d], max_thereshold_petface_box.top : [%d], max_thereshold_petface_box.right : [%d], max_thereshold_petface_box.bottom : [%d]", overlap_pet, max_petface_threshold, max_thereshold_petface_box.location.left, max_thereshold_petface_box.location.top, max_thereshold_petface_box.location.right, max_thereshold_petface_box.location.bottom);
                    #ifdef DUMPBOX
                        LOGV("remove_multi_petface_with_in_pet_boxes box score : [%f], left : [%d], top : [%d], right : [%d], bottom :[%d]", boxes[k].score, boxes[k].location.left,boxes[k].location.top,boxes[k].location.right, boxes[k].location.bottom);
                    #endif
                    if(overlap_pet != -1){
                        if(max_petface_threshold < boxes[k].score){
                            max_petface_threshold = boxes[k].score;
                            max_petface_threshold_index = k;
                            #ifdef DUMPBOX
                                LOGV("remove_multi_petface_with_in_pet_boxes updating petface overlap_pet : [%d], max_petface_threshold : [%f],  max_petface_threshold_index : [%d]", overlap_pet, max_petface_threshold, max_petface_threshold_index);
                            #endif
                        }
                    }
                }
            }
            #ifdef DUMPBOX
                LOGV("remove_multi_petface_with_in_pet_boxes final petface overlap_pet : [%d], max_petface_threshold : [%f], max_petface_threshold_index : [%d]", overlap_pet, max_petface_threshold, max_petface_threshold_index);
            #endif
            if(overlap_pet != -1 && max_petface_threshold > 0.0f && max_petface_threshold_index != -1){
                #ifdef DUMPBOX
                    LOGV("remove_multi_petface_with_in_pet_boxes adding the petface");
                #endif
                filtered_boxes.push_back(boxes[max_petface_threshold_index]);
            }            
        }
    }
    return filtered_boxes;
}