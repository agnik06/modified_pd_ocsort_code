/**
 * @file   pet_detector.h
 * @brief  Object Detector for Generic OD.
 *
 * This class provides object detection APIs for Generic OD.
 */

#ifndef __PET_DETECTOR__
#define __PET_DETECTOR__

namespace vizpet{
class PetDetector{
public:
    enum class Mode{
        SALIENCY = 0,
        SALIENCY_SYSTEM = 1,
        SALIENCY_CUSTOM = 2
    };

    /**
     * Supported image formats
     */
    enum class ImageFormat{
        NV21 = 0,
        BGR  = 1,
        RGBA = 2,
        NV12 = 3,
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

    /**
     * Details of ROI from detector
     */
    struct BoxInfo{
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
    };

  

    /**
     * @brief Initializes Object detection with callback
     *
     * @param[in] Invokes Callback on succcessful initialization
     * @param[in] Mode of operation
     * @param[in] Path of model in device
     *
     * @return bool status of initialization
     */
    bool initialize(void (*cb)(bool status),Mode mode, char* model_base_path);

    /**
     * @brief Initializes Object detection
     *
     * @param[in] Mode of operation
     * @param[in] Path of model in device
     *
     * @return bool status of initialization
     */
    bool initialize(Mode mode, char* model_base_path);

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
    bool execute(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format);

    /**
     * @brief Number of boxes on inference
     *
     * @param[in] none
     *
     * @return number of ROI boxes
     */
    int  get_n_boxes();
    /**
     * @brief ROI box details
     *
     * @param[in] box_infos contains the details of ROI
     * @param[in] size of box_infos
     *
     * @return bool status
     */
    bool get_box_info(BoxInfo box_infos[], int n);

  //TRACKING
    /**
     * Details of ROI from tracker
     */
    struct RECT{
        int left;
        int top;
        int right;
        int bottom;
    };
    /**
     * More Details of ROI from tracker
     */
    struct TrackBoxInfo{
        TagCategory tag_class; 
        int ID;
        char* tag;
        RECT rect;
        float score;
    };

    /**
     * Stores the tracking results
     */
    struct trkRes{
        int numOfBoxes;
        TrackBoxInfo Box[100];
    };

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
    bool execute_with_tracking(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format);
    
    /**
     * @brief execute_starttracking starts detection and tracking of the objects
     *
     * @param[in] image_buffer refers to the input image buffer
     * @param[in] width of input buffer
     * @param[in] height of input buffer
     * @param[in] image_rotation reflects the rotation angle of image
     * @param[in] orientation reflects the orientation of image
     * @param[in] image_format input format NV21/BGR
     * @param[in] touch_x, touch_y currently padded as -1
     * @param[in] refers to the output of tracking info
     *
     * @return bool status
     */
    bool execute_starttracking(char* image_buffer, 
                    int width, int height, 
                    int image_rotation,
                    ImageFormat image_format, 
                    int touch_x, int touch_y, trkRes& outroi_info);
    /**
     * @brief execute_keeptracking is invoked post start tracking
     *
     * @param[in] image_buffer refers to the input image buffer
     * @param[in] image_rotation reflects the rotation angle of image
     * @param[in] refers to the output of tracking info
     *
     * @return bool status
     */
    bool execute_keeptracking(char* image_buffer, int image_rotation, trkRes& outroi_info);
    /**
     * @brief stoptracking stops the tracking
     *
     * @param[in] none
     *
     * @return void
     */

    void reset_with_tracking();
    /**
     * @brief reset the tracking corresponds to execute_with_tracking API
     *
     * @param[in] none
     *
     * @return void
     */

    bool stoptracking();
    /**
     * @brief stoptracking stops the tracking
     *
     * @param[in] none
     *
     * @return void
     */
    void resettracking();
    /**
     * @brief [Constuctor]Creates Singleton instance of PetDetector
     *
     * @param[in] none
     *
     * @return none
     */
    static PetDetector* Create();
    /**
     * @brief Destroy given instance PetDetector
     *
     * @param[in] PetDetector to delete
     *
     * @return none
     */
    static void Destroy(PetDetector* image_segmenter);

};
} //namespace vizpet
#endif //__PET_DETECTOR__
