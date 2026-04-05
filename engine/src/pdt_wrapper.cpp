/**
 * @file   pet_detector_wrapper.cpp
 * @brief  This file implements the ObjectDetector Interface in form of a wrapper
 *
 * This file implements ObjectDetector Interface to be used by end api's in form of a wrapper
 */

#include "pet_detector_wrapper.h"
#include <string>
#include <vector>
#include "logger/logger.hpp"
#include "pet_detector.h"
#include "pdt_impl.hpp"

/**
 * @brief Constructor
 * @details Pet detector wrapper instance is created
 *
 * @param[in] None
 *
 * @return void
 */
PetDetectorWrapper* __attribute__((visibility("default"))) PetDetectorWrapper_create() {
	PetDetectorWrapper* dli_handle = nullptr;
	try {
		dli_handle = reinterpret_cast<PetDetectorWrapper*>(vizpet::PetDetector::Create());
	}
	catch(std::exception& ex) {
		LOGV("VZ Debug: PetDetectorWrapper_create: can't create PetDetectorWrapper: "
			 " exception: %s", ex.what());
		return nullptr;
	}
  	return dli_handle;
}

void __attribute__((visibility("default"))) PetDetectorWrapper_destroy(PetDetectorWrapper* handle) {
  //delete reinterpret_cast<viz::ObjectDetector*>(handle);
  vizpet::PetDetector::Destroy(reinterpret_cast<vizpet::PetDetector*>(handle));
}

/**
 * @brief Initializes the Object Detector
 * @details Initializes the Pet object detector with mode, basepath and callback
 *
 * @param[in] takes the callback, mode and basepath
 * @param[in] takes the mode
 *
 * @return bool status
 */
bool __attribute__((visibility("default"))) PetDetectorWrapper_initialize_Cb(PetDetectorWrapper* handle,void (*cb)(bool status),PET_OD_PARAM_WRAPPER params, char* model_base_path){
    LOGV("VZ Debug: PetDetectorWrapper_initialize_Cb model path c wrapper=[%s]", std::string(model_base_path).c_str());
    vizpet::PetDetector::Mode MODE_Od= static_cast<vizpet::PetDetector::Mode> (params.mode);
    return reinterpret_cast<vizpet::PetDetector*>(handle)->initialize(cb,MODE_Od, model_base_path);
}

/**
 * @brief Performs inference on an image buffer
 * @details Handles image format NV21 and NV12
 *
 * @param[in] image_buffer defines the image buffer
 * @param[in] width of image
 * @param[in] height of image   
 * @param[in] image_rotation defines rotation angle of image
 * @param[in] image_format of given image(one of types in ImageFormat)
 *
 * @return bool status of inference
 */

bool __attribute__((visibility("default"))) PetDetectorWrapper_execute(PetDetectorWrapper* handle,char* image_buffer, int width, int height, int image_rotation, PET_OD_PARAM_WRAPPER params){
    LOGV("VZ Debug: PetDetectorWrapper_execute c wrapper" );
    vizpet::PetDetector::ImageFormat imageFormat= static_cast<vizpet::PetDetector::ImageFormat> (params.imageFormat);
    return reinterpret_cast<vizpet::PetDetector*>(handle)->execute(image_buffer, width, height, image_rotation, imageFormat);
}

/**
 * @brief Interface to bet the ROI bixes
 * @details extracts all the boxes post detection
 *
 * @param[in] none
 *
 * @return number of boxes
 */ 
int __attribute__((visibility("default"))) PetDetectorWrapper_get_n_boxes(PetDetectorWrapper* handle){
    LOGV("VZ Debug: PetDetectorWrapper_get_n_boxes c wrapper");
    return reinterpret_cast<vizpet::PetDetector*>(handle)->get_n_boxes();
}

/**
 * @brief ROI box details
 * @details extracts all the boxes post detection
 *
 * @param[in] box_infos contains the details of ROI
 * @param[in] size of box_infos
 *
 * @return bool status
 */
bool __attribute__((visibility("default"))) PetDetectorWrapper_get_box_info(PetDetectorWrapper* handle,BoxInfo box_infos[], int n){
    LOGV("VZ Debug: PetDetectorWrapper_get_box_info c wrapper" );
    return reinterpret_cast<vizpet::PetDetector*>(handle)->get_box_info(reinterpret_cast<vizpet::PetDetector::BoxInfo*> (box_infos), n); 
}