#include <jni.h>

#include "pet_detector.h"
#include <memory>
#include "json.hpp"
#include "jni_wrapper_utils.hpp"
#include "opencv2/opencv.hpp"
#include "logger/logger.hpp"
#include "logger/timer.hpp"
#include <string.h>

#define JNI_FUNC(name) Java_vizinsight_atl_object_1detector_VZObjectDetector_##name
using json = nlohmann::json;

extern "C"{

JavaVM* jvm;
jobject main_obj; 

void init_cb(bool init_status) { 
  LOGV("VZ Debug: Initialization completed JNI mode");
  try{
    JNIEnv* main_env = NULL;
    jint get_env_status;
    jint attach_status = JNI_OK;
    jobject obj = main_obj;
    if(jvm != NULL){
      get_env_status = jvm->GetEnv((void**)&main_env, JNI_VERSION_1_6);//VZ Debug: Exception
      if(get_env_status == JNI_EDETACHED){
        attach_status = jvm->AttachCurrentThread(&main_env, NULL);
        if(attach_status != JNI_OK){
          LOGV("VZ Debug: Failed to attach current thread to jvm");
        }
      }
      else if(get_env_status == JNI_EVERSION){
          LOGV("VZ Debug: Failed to attach current thread to jvm as he JNI version is not supported");
      }
    }
    if(main_env != NULL && obj != NULL && attach_status == JNI_OK){
      jclass cls = main_env->GetObjectClass(obj);
      jmethodID init_cb_id = main_env->GetMethodID(cls, "callback", "(I)V");
      main_env->CallVoidMethod(obj, init_cb_id, init_status);
    }
    if(jvm != NULL && get_env_status == JNI_EDETACHED){
      jvm->DetachCurrentThread();
    }
  } catch (std::exception& ex) {
    LOGV("VZ Debug: Exception %s", ex.what());
  }
}



// vizpet::PetDetector* object_detector;
JNIEXPORT jboolean JNICALL JNI_FUNC(initializeJNI)(JNIEnv *env, jobject obj, jint mode_num, jstring model_base_path_str) {
    vizpet::PetDetector* object_detector = vizpet::PetDetector::Create();
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      set_engine_reference(env, obj, object_detector);
    }else{
      set_system_engine_reference(env,obj,object_detector);
    }
    env->GetJavaVM(&jvm);
    auto start_time = CURRENT_TIME;
    char* model_base_path = (char*) env->GetStringUTFChars(model_base_path_str, NULL);
    
    main_obj=env->NewGlobalRef(obj);
    bool init_status = object_detector->initialize(init_cb,mode, model_base_path);
    LG<<"VZ Debug: Time taken for initialization: "<< CURRENT_TIME - start_time<< " ms";
    env->ReleaseStringUTFChars(model_base_path_str, model_base_path);
    return init_status;
}

JNIEXPORT jboolean JNICALL JNI_FUNC(executeJNI)(JNIEnv *env, jobject obj, jobject image_bitmap_buffer,
        jint width, jint height, jint image_rotation, jint image_format_num, jint mode_num) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }
            
    auto start_time = CURRENT_TIME;
    char* image_buffer = (char*) env->GetDirectBufferAddress(image_bitmap_buffer);
    vizpet::PetDetector::ImageFormat image_format = static_cast<vizpet::PetDetector::ImageFormat>(image_format_num);
    bool execute_status = object_detector->execute(image_buffer, width, height, image_rotation, image_format);
    LG<<"VZ Debug: Time taken for execution: "<< CURRENT_TIME - start_time<<" ms";

    return execute_status;
}

JNIEXPORT jboolean JNICALL JNI_FUNC(executeWithTrackJNI)(JNIEnv *env, jobject obj, jobject image_bitmap_buffer,
        jint width, jint height, jint image_rotation, jint image_format_num, jint mode_num) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }
            
    auto start_time = CURRENT_TIME;
    char* image_buffer = (char*) env->GetDirectBufferAddress(image_bitmap_buffer);
    vizpet::PetDetector::ImageFormat image_format = static_cast<vizpet::PetDetector::ImageFormat>(image_format_num);
    bool execute_status = object_detector->execute_with_tracking(image_buffer, width, height, image_rotation, image_format);
    LG<<"VZ Debug: executeWithTrackJNI Time taken for execution: "<< CURRENT_TIME - start_time<<" ms";

    return execute_status;
}

JNIEXPORT jstring JNICALL JNI_FUNC(executeStartTackingJNI)(JNIEnv *env, jobject obj, jobject image_bitmap_buffer,
        jint width, jint height, jint image_rotation, jint orientation, jint image_format_num, jint mode_num, int touchX, int touchY) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }
            
    auto start_time = CURRENT_TIME;
    char* image_buffer = (char*) env->GetDirectBufferAddress(image_bitmap_buffer);
    vizpet::PetDetector::ImageFormat image_format = static_cast<vizpet::PetDetector::ImageFormat>(image_format_num);
	vizpet::PetDetector::trkRes trackRes;
    bool execute_status = object_detector->execute_starttracking(image_buffer, width, height, image_rotation, image_format,touchX, touchY, trackRes);
    LG<<"VZ Debug: Time taken for execution: "<< CURRENT_TIME - start_time<<" ms";
	
	std::string track_info_json_str = "";
	if(execute_status){
		track_info_json_str = convert_track_info_to_json_string(trackRes);
	}
	 LG<<"VZ Debug: Botrack_info_json_strxInfo :"<< track_info_json_str;
    jstring track_info_json_utf_str = env->NewStringUTF(track_info_json_str.c_str());
    return track_info_json_utf_str;
}

JNIEXPORT jstring JNICALL JNI_FUNC(executeKeepTrackingJNI)(JNIEnv *env, jobject obj, jobject image_bitmap_buffer,
        jint image_rotation, int mode_num) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }
            
    auto start_time = CURRENT_TIME;
    char* image_buffer = (char*) env->GetDirectBufferAddress(image_bitmap_buffer);
	vizpet::PetDetector::trkRes trackRes;
    bool execute_status = object_detector->execute_keeptracking(image_buffer, image_rotation, trackRes);
    LG<<"VZ Debug: Time taken for execution: JNI :: "<< CURRENT_TIME - start_time<<" ms";
    std::string track_info_json_str = "";
	if(execute_status){
		track_info_json_str = convert_track_info_to_json_string(trackRes);
	}
	 LG<<"VZ Debug: Botrack_info_json_strxInfo :"<< track_info_json_str;
    jstring track_info_json_utf_str = env->NewStringUTF(track_info_json_str.c_str());
    return track_info_json_utf_str;
}

JNIEXPORT jstring JNICALL JNI_FUNC(getBoxInfosJNI)(JNIEnv *env, jobject obj, jint mode_num) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }

    auto start_time = CURRENT_TIME;
    
    int n_boxes = object_detector->get_n_boxes();
    vizpet::PetDetector::BoxInfo box_infos[n_boxes]; 

    bool extraction_staus = object_detector->get_box_info(box_infos, n_boxes);
    
    if(!extraction_staus){
        return env->NewStringUTF("");
    }
    std::string result_info_json_str = convert_box_info_to_json_string(box_infos, n_boxes);
    LG<<"VZ Debug: Time taken for result-extraction1: "<< CURRENT_TIME - start_time<<" ms";
    LG<<"VZ Debug: BoxInfo :"<< result_info_json_str;
    jstring result_info_json_utf_str = env->NewStringUTF(result_info_json_str.c_str());
    return result_info_json_utf_str;
}

JNIEXPORT void JNICALL JNI_FUNC(releaseJNI)(JNIEnv *env, jobject obj, jint mode_num) {
    vizpet::PetDetector* object_detector;
    vizpet::PetDetector::Mode mode = static_cast<vizpet::PetDetector::Mode>(mode_num);
    if(mode == vizpet::PetDetector::Mode::SALIENCY){
      object_detector = get_engine_reference(env, obj);
    }else{
      object_detector = get_system_engine_reference(env, obj);
    }

    auto start_time = CURRENT_TIME;
    if(object_detector != NULL){
        vizpet::PetDetector::Destroy(object_detector);
        object_detector = NULL;
    }
    LG<<"VZ Debug: Time taken for release: "<< CURRENT_TIME - start_time<<" ms";
}
        
}; //extern "C"