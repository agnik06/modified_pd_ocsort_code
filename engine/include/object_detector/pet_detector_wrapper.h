/**
 * @file   pet_detector_wrapper.h
 * @brief  Pet Detector Wrapper for Pet OD.
 *
 * This class provides object detection APIs for Pet OD in form of a wrapper
 */

#ifndef __PET_DETECTOR_WRAPPER_HPP__
#define __PET_DETECTOR_WRAPPER_HPP__

#include <stdint.h>
#include <vector>
#include <string>


#ifdef __cplusplus
extern "C" {
#endif
struct PetDetectorWrapper;
typedef PetDetectorWrapper PetDetectorWrapper;

struct PET_OD_PARAM_WRAPPER {
    /**
     * Modes supported
     */
    typedef enum {
			SALIENCY = 0,
            SALIENCY_SYSTEM = 1,
            SALIENCY_CUSTOM = 2

	}Mode;
    /**
     * Supported image formats
     */
    typedef enum {
		NV21 = 0,
        BGR  = 1,
        RGBA = 2,
        NV12 = 3,

	}ImageFormat;

    Mode mode;
    ImageFormat imageFormat;

};
    /**
     * Categories supported
     */
enum class TagCategory {
        TAG_INVALID = 0,
        TAG_OBJECT = 1,
        TAG_PERSON = 2,
        TAG_PETFACE = 8,
        TAG_PET = 10,
        TAG_PET_EYES = 14
    };

typedef struct{
        TagCategory tag_class;
        char* tag;
        int left;
        int top;
        int right;
        int bottom;
        int image_width;
        int image_height;
        float score;
        int trackid;
}BoxInfo;



PetDetectorWrapper* PetDetectorWrapper_create();

/**
 * @brief Initializes Pet Object detection with callback
 *
 * @param[in] Invokes Callback on succcessful initialization
 * @param[in] Mode of operation
 * @param[in] Path of model in device
 *
 * @return bool status of initialization
 */

bool PetDetectorWrapper_initialize_Cb(PetDetectorWrapper* handle,void (*cb)(bool status),PET_OD_PARAM_WRAPPER params, char* model_base_path);

/**
 * @brief Performs inference on an image buffer
 *
 * @param[in] image_buffer defines the image buffer
 * @param[in] width of image
 * @param[in] height of image
 * @param[in] image_rotation defines rotation angle of image
 * @param[in] image_format of given image(one of types in ImageFormat)
 *
 * @return bool status of inference
 */
bool PetDetectorWrapper_execute(PetDetectorWrapper* handle,char* image_buffer, int width, int height, int image_rotation, PET_OD_PARAM_WRAPPER params);

/**
 * @brief Number of boxes on inference
 *
 * @param[in] none
 *
 * @return number of ROI boxes
 */
int PetDetectorWrapper_get_n_boxes(PetDetectorWrapper* handle);

/**
 * @brief ROI box details
 *
 * @param[in] box_infos contains the details of ROI
 * @param[in] size of box_infos
 *
 * @return bool status
 */
bool PetDetectorWrapper_get_box_info(PetDetectorWrapper* handle,BoxInfo box_infos[], int n);

/**
 * @brief Destroy given instance Pet ObjectDetector
 *
 * @param[in] ObjectDetector to delete
 *
 * @return none
 */
void PetDetectorWrapper_destroy(PetDetectorWrapper* handle);



}

#endif 