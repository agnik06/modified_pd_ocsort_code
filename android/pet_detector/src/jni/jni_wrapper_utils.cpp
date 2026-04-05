#include "jni_wrapper_utils.hpp"
#include "logger/logger.hpp"
#include "opencv2/opencv.hpp"

std::string convert_box_info_to_json_string(vizpet::PetDetector::BoxInfo box_infos[], int n_boxes){
    json j_arr = json::array();
    for(int i=0; i<n_boxes; i++){
        json j_obj;
        j_obj["track_id"] = box_infos[i].trackid;
        j_obj["tag"]    = std::string(box_infos[i].tag); 
        j_obj["tag_id"] = (int)box_infos[i].tag_class;
        j_obj["left"]   = box_infos[i].left;
        j_obj["top"]    = box_infos[i].top;
        j_obj["right"]  = box_infos[i].right;
        j_obj["bottom"] = box_infos[i].bottom;
        j_obj["image_width"]  = box_infos[i].image_width;
        j_obj["image_height"] = box_infos[i].image_height;
        j_obj["score"] = box_infos[i].score;
        j_arr.push_back(j_obj);
    }
    return j_arr.dump();
}

std::string convert_track_info_to_json_string(vizpet::PetDetector::trkRes trackRes){
	int n_boxes = (int)trackRes.numOfBoxes;
    LG<<"VZ Debug: convert_track_info_to_json_string";
	vizpet::PetDetector::TrackBoxInfo track_infos[n_boxes];
	
	for(int i =0;i<n_boxes;i++){
		track_infos[i] = trackRes.Box[i];
	}
	
	json j_arr = json::array();
	for(int i=0; i<n_boxes; i++){
		json j_obj;
        LG<<"VZ Debug: convert_track_info_to_json_string left : "<<(int)track_infos[i].rect.left << " ClassID:: " << (int)track_infos[i].tag_class <<" top : "<<(int)track_infos[i].rect.top<<" right : "<<(int)track_infos[i].rect.right<<" bottom : "<<(int)track_infos[i].rect.bottom;
        //j_obj["tag"]    = std::string(track_infos[i].tag);
        j_obj["tag_id"] = (int)track_infos[i].tag_class;
		j_obj["id"] = (int)track_infos[i].ID;
        j_obj["left"]   = (int)track_infos[i].rect.left;
        j_obj["top"]    = (int)track_infos[i].rect.top;
        j_obj["right"]  = (int)track_infos[i].rect.right;
        j_obj["bottom"] = (int)track_infos[i].rect.bottom;
        j_obj["score"] = track_infos[i].score;
        j_arr.push_back(j_obj);

        //LG<<"VZ Debug: tag : "<< std::string(track_infos[i].tag);
    }
    LG<<"VZ Debug: exit convert_track_info_to_json_string";
    return j_arr.dump();
    
}

vizpet::PetDetector* get_engine_reference(JNIEnv *env, jobject obj){
    jclass   cls      = env->GetObjectClass(obj);
    const char* field_signature = "[Lvizinsight/atl/object_detector/VZObjectDetector;";
    jfieldID field_id = env->GetFieldID(cls, "engine_reference", "J");
    jlong    lValue   = env->GetLongField(obj, field_id);
    return (vizpet::PetDetector*)lValue;
}

void set_engine_reference(JNIEnv *env, jobject obj, vizpet::PetDetector* object_detector){
    jclass cls        = env->GetObjectClass(obj);
    const char* field_signature = "[Lvizinsight/atl/object_detector/VZObjectDetector;";
    jfieldID field_id = env->GetFieldID(cls, "engine_reference", "J");
    env->SetLongField(obj, field_id, (jlong)object_detector);
}

vizpet::PetDetector* get_system_engine_reference(JNIEnv *env, jobject obj){
    jclass   cls      = env->GetObjectClass(obj);
    const char* field_signature = "[Lvizinsight/atl/object_detector/VZObjectDetector;";
    jfieldID field_id = env->GetFieldID(cls, "system_engine_reference", "J");
    jlong    lValue   = env->GetLongField(obj, field_id);
    return (vizpet::PetDetector*)lValue;
}

void set_system_engine_reference(JNIEnv *env, jobject obj, vizpet::PetDetector* object_detector){
    jclass cls        = env->GetObjectClass(obj);
    const char* field_signature = "[Lvizinsight/atl/object_detector/VZObjectDetector;";
    jfieldID field_id = env->GetFieldID(cls, "system_engine_reference", "J");
    env->SetLongField(obj, field_id, (jlong)object_detector);
}
