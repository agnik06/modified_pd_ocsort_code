/**
 * @file   pdt_impl.hpp
 * @brief  This file implements the Pet ObjectDetector Interface
 *
 * This file implements Pet ObjectDetector Interface to be used by end api's
 */

#ifndef __PET_DETECTORR_IMPL__
#define __PET_DETECTORR_IMPL__

#include "pet_detector.h"
#include "core/config.hpp"
#include "core/framework/bounding_box.hpp"
#include "core/framework/rectangle.hpp"
//#include "core/utils/rectangle_utils.hpp"
#include "tagger/base_tagger.hpp"
#include "opencv2/opencv.hpp"
#include "logger/logger.hpp"
#include "tagger/factory/tagger_factory.hpp"
#include "utils/image_utils.hpp"
#include "utils/rectangle_utils.hpp"
#include "utils/bounding_box_utils.hpp"
#include "modules/detector/filterer/roi_utils.hpp"
//#include "utils/homography_analyzer.hpp"

#include "modules/detector/tracker_wrapper/tracker/OCSort.hpp"

#include <ctime>
#include <string>
#include <mutex>
#include <memory>


class PDTImpl : public vizpet::PetDetector{

    enum Version{
	    UNKNOWN = 0,
        V1 = 1,
        V2 = 2,
        V2_MULTI_FRAME = 3,

    };

public:
    PDTImpl();
    ~PDTImpl();
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
     * @brief Initializes tagger callback
     *
     * @return none
     */
    void initialize_tagger_callback();
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
    /**
     * @brief Corrects the ROI boxes
     *
     * @param[in] width and height of the image
     * @param[in] angular rotation
     * @param[in] width and height of image
     *
     * @return bool status
     */
    void correct_roi_boxes(int target_width,int target_height,const int angular_rotation_clockwise,int width,int height);
    void (*init_cb)(bool);
    std::vector<vizpet::BoundingBox> getresultboxes(){ return result_boxes;}

    void generate_association_ids(char* image_buffer,std::vector<vizpet::BoundingBox> boxes,int width, int height);

    //Tracking interfaces

    bool inittracking();
    void deinittracking();
    bool execute_starttracking(char* image_buffer, 
                    int width, int height, 
                    int image_rotation, int orientation,
                    ImageFormat image_format, 
                    int touch_x, int touch_y, trkRes& outroi_info);

    bool execute_keeptracking(char* image_buffer, int image_rotation, trkRes& outroi_info);

    bool stoptracking();
    void resettracking();
    bool execute_with_tracking(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format);
    void reset_with_tracking();
    float get_overlap_between_boxes(vizpet::Rectangle box1, vizpet::Rectangle box2);
    
#ifdef TEMPAVG
    //vizpet::Rectangle ComputeMovingAverage(int index, int left, int top, int right, int bottom,int trackid);
    void reset_execute_ocsort_tracker();
#endif

//Homography Analysis
  //std::shared_ptr<HomograhyAnalyzer> homography_analyzer;
  //bool check_image_continuity(cv::Mat image);

private:
    //for moving average
    #ifdef TEMPAVG
        struct movingAvg{
            int counter;
            int trackid;
            vizpet::Rectangle smoothed_box;
            struct less_than{
                bool operator()(const movingAvg &a, int id){
                   return a.trackid < id;
            }
        };
    };
    int uodMovingAverageInterval = 4;
    std::vector<movingAvg> movingAvgVec;
    
    #endif
    ocsort::OCSort* execute_ocsort_tracker;
    //for tracking
    bool trackingstarted;
    bool trackingstopped;
    int m_imgWidth;
    int m_imgheight;
    int m_rotation;
    int m_orientation;
    ImageFormat m_image_format;
    int m_touch_x;
    int m_touch_y;
    ocsort::OCSort* ocsort_tracker;
    bool execute_with_track;

    int m_frame_number;
    const int PETFACE_ID = 3;
    const int PET_ID = 5;
    const int PET_EYES_ID = 6;
    int prev_frame_pet_boxes;

    char* m_image_buffer;

    int m_width;
    int m_height;
    int original_image_cnt=0;
    int original_image_cnt1=0;
    int detect_image_cnt = 0;
    int detect_org_img_cnt = 0;
    int detect_track_img_cnt =0;
    int final_result_cnt =0;
    int track_detect_image_cnt = 0;
    int merge_track_detect_image_cnt = 0;
    int tmpavg_track_detect_image_cnt = 0;
    int wo_resize_detect_img_cnt =0;
    int track_detect_track_image_cnt = 0;
    int original_image_post_cnt = 0;
    int post_assocaion_image_cnt =0;
    int pre_assocaion_image_cnt=0;
    //for detection
    bool init_done;
    TaggerInitCallback tagger_init_cb;
    vizpet::PetDetector::Mode mode;
    std::string model_base_path;

    std::vector<vizpet::BoundingBox> result_boxes;
    std::vector<vizpet::BoundingBox> prev_frame_pet_result_boxes;

    std::shared_ptr<Config> config;
    std::shared_ptr<BaseTagger> tagger;

    std::mutex initialize_mtx;
    std::mutex execute_mtx;
    std::mutex execute_tracking_mtx;
    std::mutex result_extraction_mtx;

};

#endif //__PET_DETECTORR_IMPL__
