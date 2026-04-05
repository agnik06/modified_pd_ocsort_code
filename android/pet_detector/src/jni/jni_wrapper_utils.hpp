#include <jni.h>

#include "pet_detector.h"
#include "json.hpp"
#include <memory>
#include <string>

using json = nlohmann::json;

std::string convert_box_info_to_json_string(vizpet::PetDetector::BoxInfo box_infos[], int n_boxes);
vizpet::PetDetector* get_engine_reference(JNIEnv *env, jobject obj);
void set_engine_reference(JNIEnv *env, jobject obj, vizpet::PetDetector* object_detector);

vizpet::PetDetector* get_system_engine_reference(JNIEnv *env, jobject obj);
void set_system_engine_reference(JNIEnv *env, jobject obj, vizpet::PetDetector* object_detector);
std::string convert_track_info_to_json_string(vizpet::PetDetector::trkRes trackRes);