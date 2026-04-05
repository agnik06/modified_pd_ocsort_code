/**
 * @file   light_object_detector.cpp
 * @brief  This file implements the LightObjectDetector Interface
 *
 * This file implements LightObjectDetector Interface to be used by end api's
 */

#include "pet_detector.h"
#include "pdt_impl.hpp"

namespace vizpet{    

bool __attribute__((visibility("default"))) PetDetector::initialize(void (*cb)(bool status),Mode mode, char* model_base_path){
    return ((PDTImpl*)this)->initialize(cb,mode, model_base_path);
}

bool __attribute__((visibility("default"))) PetDetector::initialize(Mode mode, char* model_base_path){
    return ((PDTImpl*)this)->initialize(mode, model_base_path);
}

bool __attribute__((visibility("default"))) PetDetector::execute(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format){
    return ((PDTImpl*)this)->execute(image_buffer, width, height, image_rotation, image_format);
}

bool __attribute__((visibility("default"))) PetDetector::execute_with_tracking(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format){
    return ((PDTImpl*)this)->execute_with_tracking(image_buffer, width, height, image_rotation, image_format);
}

int __attribute__((visibility("default")))  PetDetector::get_n_boxes(){
    return ((PDTImpl*)this)->get_n_boxes();    
}

bool __attribute__((visibility("default"))) PetDetector::get_box_info(BoxInfo box_infos[], int n){
    return ((PDTImpl*)this)->get_box_info(box_infos, n);    
}

bool __attribute__((visibility("default"))) PetDetector::execute_starttracking(char* image_buffer, 
                int width, int height, 
                int image_rotation,
                //int orientation,
                ImageFormat image_format, 
                int touch_x, int touch_y, trkRes& outroi_info){
    return ((PDTImpl*)this)->execute_starttracking(image_buffer, width, height, image_rotation, 
                                    -1, image_format, touch_x, touch_y, outroi_info);
}

bool __attribute__((visibility("default"))) PetDetector::execute_keeptracking(char* image_buffer, int image_rotation, trkRes& outroi_info){
    return ((PDTImpl*)this)->execute_keeptracking(image_buffer, image_rotation, outroi_info);

}

bool __attribute__((visibility("default"))) PetDetector::stoptracking(){
    return ((PDTImpl*)this)->stoptracking();

}
void __attribute__((visibility("default"))) PetDetector::resettracking(){
    return ((PDTImpl*)this)->resettracking();

}

void __attribute__((visibility("default"))) PetDetector::reset_with_tracking(){
    return ((PDTImpl*)this)->reset_with_tracking();

}

} //namespace vizpet

vizpet::PetDetector* __attribute__((visibility("default"))) vizpet::PetDetector::Create(){
    return (PetDetector*) new PDTImpl();
}

void __attribute__((visibility("default"))) vizpet::PetDetector::Destroy(PetDetector* object_detector){
    delete (PDTImpl*) (object_detector);
}
