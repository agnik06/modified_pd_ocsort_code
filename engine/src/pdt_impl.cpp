/**
 * @file   pdt_impl.cpp
 * @brief  This file implements the Pet ObjectDetector Interface
 *
 * This file implements Pet ObjectDetector Interface to be used by end api's
 */

#include "pdt_impl.hpp"
#include "sys/stat.h"
#include <limits>
#include <set>


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define PET_OD_BUILD_INFO __PET_OD_VERSION__


/**
 * @brief Constructor
 * @details Pet detector constructor
 *
 * @param[in] None
 *
 * @return void
 */
PDTImpl::PDTImpl(){
    LG<<"VZ Debug: Using Pet ObjectDetector Built on : "<< std::string(__DATE__) << " " << std::string(__TIME__) << " IST";
    // LOGV("VZ Debug: Version Info: %s",PET_OD_BUILD_INFO);
    // LOGV("VZ Debug: Pet ObjectDetector version built: %s", std::string(PET_OD_BUILD_INFO).c_str());
    //homography_analyzer = nullptr;
    execute_ocsort_tracker = 0;
    #ifdef TEMPAVG
      execute_ocsort_tracker = 0;
      m_frame_number = 0;
    #endif
}
PDTImpl::~PDTImpl(){
    std::lock_guard<std::mutex> guard_initialize(this->initialize_mtx);
    std::lock_guard<std::mutex> guard_execute(this->execute_mtx);
    std::lock_guard<std::mutex> guard_track_execute(this->execute_tracking_mtx);
    std::lock_guard<std::mutex> guard_extraction(this->result_extraction_mtx);
    tagger = nullptr;
    #ifdef DUMPBOX
      LOGV("VZ Debug: ~PDTImpl execute_with_track = [%d]", execute_with_track);
    #endif
     #ifdef TEMPAVG
      reset_execute_ocsort_tracker();
    #endif
    /*if(homography_analyzer != nullptr){
      homography_analyzer->clear();
    }
    homography_analyzer = nullptr;*/
    
    LG<<"Deleting Pet ObjectDetector...............";
}

#ifdef TEMPAVG
void PDTImpl::reset_execute_ocsort_tracker(){
    if(execute_ocsort_tracker){
      delete execute_ocsort_tracker;
      execute_ocsort_tracker = 0;
    }
    m_frame_number = 0;
}
#endif

/**
 * @brief Initiazess the tagger callback
 * @details tagger callback initialization
 *
 * @param[in] None
 *
 * @return void
 */
void PDTImpl::initialize_tagger_callback(){
  tagger_init_cb = [this](bool status){
    if(!status){
      LOGV("VZ Debug: Initilization of Tagger is failed");
      this->init_done = false;
    }
    else{
      this->init_done = true;
    }

    LOGV("VZ Debug: Initializing tagger done for %s", config->getModeAsString().c_str());
    try {
      if(this->init_cb != nullptr){
        this->init_cb(this->init_done);
      }
    } catch (std::exception& ex) {
        LOGV("VZ Debug: Exception - %s", ex.what());
      this->init_done = false;
    }
  };
}


/**
 * @brief Initialization function with mode and model base path
 * @details Initilizes teh model with mode and basepath
 *
 * @param[in] Mode
 * @param[in] Model base path
 *
 * @return bool status
 */
bool PDTImpl::initialize(Mode mode, char* model_base_path){
    std::lock_guard<std::mutex> guard(this->initialize_mtx);

#ifdef ANDROID_ARM_NEON
    LG<<"Initializing armneon used in preproc for nv21...";
#else
    LG<<"Initializing ...";
#endif
    LG<<"Models path :"<<model_base_path;
    config = std::make_shared<Config>();
    config->setMode(mode);
    config->setModelFilePath(model_base_path);
    config->addModule(ModuleProperty("aic_pet_detector", config));

    switch(config->getMode()){
      case vizpet::PetDetector::Mode::SALIENCY:
        config->setVersionFromModule("aic_pet_detector");
        break;
      case vizpet::PetDetector::Mode::SALIENCY_SYSTEM:
        config->setVersionFromModule("aic_pet_detector");
        break;
    }
    initialize_tagger_callback();
    TaggerFactory tagger_factory;
    try{
      tagger = tagger_factory.get_tagger(config,tagger_init_cb);
    }catch (std::exception& ex) {
      LG<<"VZ Debug: Exception - "<<ex.what();
      return false;
    }    
    if(tagger){
        return true;
    }
    return false;
}

/**
 * @brief Initializes teh Object Detector
 * @details Initializes the object detector with mode, basepath and callback
 *
 * @param[in] takes the callback, mode and basepath
 * @param[in] takes the mode
 *
 * @return bool status
 */
bool PDTImpl::initialize(void (*cb)(bool status),Mode mode, char* model_base_path){
    this->init_cb=cb;
    bool status=initialize(mode,model_base_path);
    #ifdef TEMPAVG
      execute_ocsort_tracker = 0;
      m_frame_number = 0;
    #endif
   /* if ( homography_analyzer == nullptr ) {
     try {
        homography_analyzer = std::make_shared<HomograhyAnalyzer>();
     } catch(cv::Exception& e) {
        LOGV("AJ Debug: opencv exception in homography  creation.");
        return false;
     } catch (...) {
        LOGV("AJ Debug: unknown exception in homography creation.");
        return false;
     }
    }*/
    prev_frame_pet_boxes = 0;
    return status;    
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

 bool confidenceComparator(vizpet::BoundingBox box1, vizpet::BoundingBox box2){
  return box2.score < box1.score;
}

/*bool PDTImpl::check_image_continuity(cv::Mat image){
  LOGV("VZ Debug: check_image_continuity");
  bool has_continuity = true;
  float th_dist = 0.3f;

  if (!homography_analyzer->has_reference_frame_multiscale()) {
    has_continuity = false;
    homography_analyzer->set_reference_frame_multiscale(image);
    homography_analyzer->set_threshold(th_dist);
  }
  else {
    float dist = 1.0f;
    try {
      dist = homography_analyzer->analyze_multiscale(image);
      LOGV("VZ Debug: check_image_continuity homography_dist\t%f", dist);
    } catch (cv::Exception& e) {
      LOGV("VZ Debug: opencv exception in homography computation");
    }
    
    if (dist > th_dist) {
      has_continuity = false;
      homography_analyzer->set_reference_frame_multiscale(image);
    }
  }
  return has_continuity;
}*/


float PDTImpl::get_overlap_between_boxes(vizpet::Rectangle box1, vizpet::Rectangle box2) {
    //Update on 11th Nov: Replaced intersection/box2_area logic with intersection/union_area logic
    return calculateIoU(box1, box2);
}

 bool PDTImpl::execute(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format){
    LOGV("execute: image_rotation : [%d], width : [%d], height : [%d], image_format: [%d]",image_rotation,width,height,image_format);
    std::lock_guard<std::mutex> guard(this->execute_mtx);
    if (!this->init_done) {
        #ifdef DUMPBOX
          LOGV("VZ Debug: Execute called without proper init completion. Execute aborted !");
        #endif
        return false;
    }
    if(tagger == nullptr) return false;

    auto start_time = CURRENT_TIME;
    execute_with_track = false;

    int target_height,target_width;
    m_image_buffer = image_buffer;
    tagger->get_target_dimen(target_height,target_width);
    m_width = width;
    m_height = height;
    cv::Mat image;
    if(image_format==vizpet::PetDetector::ImageFormat::NV12){
      //todo add neon optimization
      image = nv_convert_resize_rotate(image_buffer,width,height,target_width,target_height,image_rotation,image_format);
    }
    else if(image_format==vizpet::PetDetector::ImageFormat::NV21){
      //This is done so that image orientation is resized and corrected before its passed to tagger
      #ifdef ANDROID_ARM_NEON
            image = vizpet_arm_neon::nv21_resize_convert_rotate(image_buffer, width, height, target_width, target_height, image_rotation);
        #else
            image = nv_convert_resize_rotate(image_buffer,width,height,target_width,target_height,image_rotation,image_format);
        #endif
    }
    else{
      //rest of the image types
      image = get_bgr_mat_from_buffer(image_buffer, width, height, image_format,target_width,target_height);
      image = rotate_image(image, image_rotation);
    }
    auto preprocess_finish = CURRENT_TIME;
    LG<<"VZ Debug: pre-processing image: time taken: "<< preprocess_finish - start_time <<" ms";
    std::vector<vizpet::BoundingBox> boxes;
     #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump original boxes";
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_original/%d.jpg", original_image_cnt++);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                imwrite(buffer, image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                    
            }
            #endif //DUMPBOX
    #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              cv::Mat original_image = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, width, height, image_rotation);
              LG << "dump original boxes";
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_original_camera/%d.jpg", original_image_cnt1++);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                imwrite(buffer, original_image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                    
            }
            #endif //DUMPBOX
    boxes = tagger->get_boxes(image);
    for(auto &hooks : tagger->bounding_box_hooks){
        boxes=hooks(boxes,{});
    }
    if(!boxes.empty()){
        std::lock_guard<std::mutex> guard_extraction(this->result_extraction_mtx);
        result_boxes = boxes;
        #ifdef DUMPBOX1 //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector boxes";
              detect_image_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod/%d.jpg", detect_image_cnt);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                rectangle(image, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(0,0,255), 3);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(image,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255),3);
                imwrite(buffer, image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
              }      
            }
            #endif //DUMPBOX
            #ifdef DUMPBOX 
            for(int i =0 ;i <result_boxes.size();i++){
                LOGV("VZ Debug: execute before correct detected trackid : [%d],class: [%s], class id : [%d], score: [%f], left : [%d], top : [%d], bottom : [%d], right : [%d]", result_boxes[i].trackid, result_boxes[i].tag.c_str(), result_boxes[i].tag_id, result_boxes[i].score, result_boxes[i].location.left,result_boxes[i].location.top,result_boxes[i].location.bottom,result_boxes[i].location.right);

            }
            #endif //DUMPBOX

        correct_roi_boxes(target_width,target_height,image_rotation,width,height);
        #ifdef DUMPBOX
        for(int i =0 ;i <result_boxes.size();i++){
                LOGV("VZ Debug: execute after correct detected trackid : [%d],class: [%s], class id : [%d], score: [%f], left : [%d], top : [%d], bottom : [%d], right : [%d]", result_boxes[i].trackid, result_boxes[i].tag.c_str(), result_boxes[i].tag_id, result_boxes[i].score, result_boxes[i].location.left,result_boxes[i].location.top,result_boxes[i].location.bottom,result_boxes[i].location.right);
        }
        #endif //DUMPBOX
         
    }
    else{   
        LG<<"No boxes detected";
        result_boxes.clear();
    }
    auto execute_finish = CURRENT_TIME;
    LG<<"VZ Debug: Total Execute: time taken: "<<execute_finish - start_time <<" ms";
    return true;
}
bool compare_scores(vizpet::BoundingBox& box1, vizpet::BoundingBox& box2) {
    return box1.score > box2.score;
}

vizpet::BoundingBox find_max_score_box(std::vector<vizpet::BoundingBox>& boxes) {
    std::sort(boxes.begin(), boxes.end(), compare_scores);
    return boxes[0];
}

bool check_bounding_box_contains(vizpet::BoundingBox& box1, std::vector<vizpet::BoundingBox>& boxes){
    for(auto& box2 : boxes){
        if(box1.location.left == box2.location.left && box1.location.top == box2.location.top && box1.location.right == box2.location.right && box1.location.bottom == box2.location.bottom){
            return true;
        }
    }
    return false;
}

void PDTImpl::generate_association_ids(char* image_buffer,std::vector<vizpet::BoundingBox> boxes, int width, int height){
     std::vector<vizpet::BoundingBox> pet_boxes;
    std::vector<vizpet::BoundingBox> pet_face_boxes;
    std::vector<vizpet::BoundingBox> pet_eyes_boxes;
    std::vector<vizpet::BoundingBox> other_boxes;
    std::vector<vizpet::BoundingBox> overlapping_petface_boxes;
    std::vector<vizpet::BoundingBox> total_overlapping_petface_boxes;
    std::vector<vizpet::BoundingBox> total_overlapping_pet_eye_boxes;
    std::vector<vizpet::BoundingBox> overlapping_pet_eye_boxes;
    std::vector<vizpet::BoundingBox> pet_face_final_boxes;
    std::vector<vizpet::BoundingBox> pet_eye_final_boxes;
    std::vector<vizpet::BoundingBox> final_boxes;
    float iou_threshold = 0.80;
    for(int i =0;i<boxes.size();i++){
        if(boxes[i].tag_id == 5){
            pet_boxes.push_back(boxes[i]);
        }else if(boxes[i].tag_id == 3){
            pet_face_boxes.push_back(boxes[i]);
        }else if(boxes[i].tag_id == 6){
            pet_eyes_boxes.push_back(boxes[i]);
        }else{
            other_boxes.push_back(boxes[i]);
        }
    }
     #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            cv::Mat dumpImage_pre;
              dumpImage_pre = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, width, height, m_rotation);
            {
              LG << "dump detector boxes";
              pre_assocaion_image_cnt++;
              for(int i=0; i<boxes.size(); i++){
                if(boxes[i].tag_id == 3 || boxes[i].tag_id == 5){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/pre_association_detect/%d.jpg", pre_assocaion_image_cnt);
                //sprintf(buffer, "/sdcard/uod_det_corr/%d.jpg", pre_assocaion_image_cnt);
                rectangle(dumpImage_pre, cv::Point(boxes[i].location.left,boxes[i].location.top), cv::Point(boxes[i].location.right,boxes[i].location.bottom), cv::Scalar(0,0,255), 3);
                std::string detectorStr = (std::to_string(boxes[i].trackid)).append("_").append(std::to_string(boxes[i].tag_id)).append("_").append(std::to_string(boxes[i].score));
                putText(dumpImage_pre,detectorStr,cv::Point(boxes[i].location.left, boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255),3);
                imwrite(buffer, dumpImage_pre);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                }
              }      
            }
            #endif //DUMPBOX
    for (vizpet::BoundingBox& pet_box : pet_boxes) {
      overlapping_petface_boxes.clear();
        for(int i =0; i<pet_face_boxes.size();i++){
            int overlap = ROIUtils::check_overlap_box(pet_box.location, pet_face_boxes[i].location,0.85);
            if (overlap != -1) {
                overlapping_petface_boxes.push_back(pet_face_boxes[i]);
                total_overlapping_petface_boxes.push_back(pet_face_boxes[i]);
            }
        }
        //LOGV("generate_association_ids: overlapping_petface_boxes size : [%d]",overlapping_petface_boxes.size());
        if(overlapping_petface_boxes.size() ==1){
          if(pet_face_final_boxes.empty()){
            pet_face_final_boxes.push_back(overlapping_petface_boxes[0]);
          }else{
            if(!check_bounding_box_contains(overlapping_petface_boxes[0], pet_face_final_boxes)){
                pet_face_final_boxes.push_back(overlapping_petface_boxes[0]);
              }
          }
      
        }
        else if (overlapping_petface_boxes.size() > 1) {
            vizpet::BoundingBox merged_box = find_max_score_box(overlapping_petface_boxes);
            if(pet_face_final_boxes.empty()){
                pet_face_final_boxes.push_back(merged_box);
            }else {
              if(!check_bounding_box_contains(merged_box, pet_face_final_boxes)){
                pet_face_final_boxes.push_back(merged_box);
              }
            }
        }

    }
    for (int i =0;i < pet_face_boxes.size();i++){
        if(!check_bounding_box_contains(pet_face_boxes[i], total_overlapping_petface_boxes)){
            pet_face_final_boxes.push_back(pet_face_boxes[i]);
        }
    }
    // Propagate pet body's trackid to overlapping pet face (association only, no new IDs)
    for(int i = 0; i < pet_boxes.size(); i++){
        for(int j =0; j<pet_face_final_boxes.size(); j++){
            int overlap = ROIUtils::check_overlap_box(pet_boxes[i].location, pet_face_final_boxes[j].location,0.85);
            if(overlap != -1){
                pet_face_final_boxes[j].trackid = pet_boxes[i].trackid;
                break;
            }
        }
    }

    for (vizpet::BoundingBox& pet_face_box : pet_face_boxes) {
      overlapping_pet_eye_boxes.clear();
        for(int i =0; i<pet_eyes_boxes.size();i++){
            int overlap = ROIUtils::check_overlap_box(pet_face_box.location, pet_eyes_boxes[i].location,0.85);
            if (overlap != -1) {
                overlapping_pet_eye_boxes.push_back(pet_eyes_boxes[i]);
                total_overlapping_pet_eye_boxes.push_back(pet_eyes_boxes[i]);
            }
        }
        if(overlapping_pet_eye_boxes.size() ==1){
            pet_eye_final_boxes.push_back(overlapping_pet_eye_boxes[0]);
        }
        else if(overlapping_pet_eye_boxes.size() ==2){
            pet_eye_final_boxes.push_back(overlapping_pet_eye_boxes[0]);
            pet_eye_final_boxes.push_back(overlapping_pet_eye_boxes[1]);
        }
        else if (overlapping_pet_eye_boxes.size() > 2) {
            std::sort(overlapping_pet_eye_boxes.begin(), overlapping_pet_eye_boxes.end(), compare_scores);
            pet_eye_final_boxes.push_back(overlapping_pet_eye_boxes[0]);
            pet_eye_final_boxes.push_back(overlapping_pet_eye_boxes[1]);
        }

    }
    /*for (int i =0;i < pet_eyes_boxes.size();i++){
        if(!check_bounding_box_contains(pet_eyes_boxes[i], total_overlapping_pet_eye_boxes)){
            pet_eye_final_boxes.push_back(pet_eyes_boxes[i]);
        }
    }*/
    for(int i = 0; i < pet_face_final_boxes.size(); i++){
        for(int j =0; j<pet_eye_final_boxes.size(); j++){
            int overlap = ROIUtils::check_overlap_box(pet_face_final_boxes[i].location, pet_eye_final_boxes[j].location,0.85);
            if(overlap != -1){
                pet_eye_final_boxes[j].trackid = pet_face_final_boxes[i].trackid;
            }
        }
    }
    // Rebuild result_boxes — all boxes keep their OCSort-assigned persistent trackid
    result_boxes.clear();
    for (const auto& box : pet_boxes) {
        result_boxes.push_back(box);
    }
    for (const auto& box : pet_face_final_boxes) {
        result_boxes.push_back(box);
    }
    for (const auto& box : pet_eye_final_boxes) {
        result_boxes.push_back(box);
    }
    for (const auto& box : other_boxes) {
        result_boxes.push_back(box);
    }
    #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            cv::Mat dumpImage_post;
              dumpImage_post = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, width, height, m_rotation);
            {
              LG << "dump detector boxes";
              post_assocaion_image_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                if(result_boxes[i].tag_id == 3 || result_boxes[i].tag_id == 5){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/post_association_detect/%d.jpg", post_assocaion_image_cnt);
                //sprintf(buffer, "/sdcard/uod_det_corr/%d.jpg", pre_assocaion_image_cnt);
                rectangle(dumpImage_post, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(0,0,255), 3);
                std::string detectorStr = (std::to_string(result_boxes[i].trackid)).append("_").append(std::to_string(result_boxes[i].tag_id)).append("_").append(std::to_string(result_boxes[i].score));
                putText(dumpImage_post,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255),3);
                imwrite(buffer, dumpImage_post);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                }
              }      
            }
            #endif //DUMPBOX
}

bool PDTImpl::execute_with_tracking(char* image_buffer, int width, int height, int image_rotation, ImageFormat image_format){
    std::lock_guard<std::mutex> guard(this->execute_tracking_mtx);
    LOGV("execute_with_tracking: image_rotation : [%d], width : [%d], height : [%d], image_format: [%d]",image_rotation,width,height,image_format);
    // LOGV("execute_with_tracking: return true ");
    // return true;
    if (!this->init_done) {
        LOGV("VZ Debug: Execute called without proper init completion. Execute aborted !");
        return false;
    }
    if(tagger == nullptr) return false;

    auto start_time = CURRENT_TIME;
    execute_with_track = true;
    m_frame_number++;
    #ifdef DUMPBOX
      LOGV("execute_with_tracking : m_frame_number : [%d]",m_frame_number);
    #endif  

    if(!execute_ocsort_tracker){
      detect_org_img_cnt = 0;
      final_result_cnt = 0;

      const int frame_rate = 30; const int track_buffer = 30;
      const float track_thresh = 0.5; const float high_thresh = 0.6;
      const float match_thresh = 0.8;
      const float det_thresh = 0.5; const int delta_t = 3;
      execute_ocsort_tracker = new ocsort::OCSort(det_thresh, track_buffer, 3, track_thresh, delta_t, "iou", 0.2, false);
    #ifdef TEMPAVG
      movingAvgVec.clear();
    #endif
    }

    int target_height,target_width;
    m_image_buffer = image_buffer;
    tagger->get_target_dimen(target_height,target_width);
    m_width = width;
    m_height = height;
    m_rotation = image_rotation;
    m_image_format = image_format;
    cv::Mat image;
    #ifdef DUMPBOX1 //Input image with detector boxes ; need to keep tese dumps on always
            {
              cv::Mat dump_image;
              dump_image = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, target_width, target_height, image_rotation);
              LG << "dump original boxes";
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_original_dump/%d.jpg", original_image_cnt++);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                imwrite(buffer, dump_image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                    
            }
            #endif //DUMPBOX
    //image_rotation = image_rotation+90;
    if(image_format==vizpet::PetDetector::ImageFormat::NV12){
      //todo add neon optimization
      image = nv_convert_resize_rotate(image_buffer,width,height,target_width,target_height,image_rotation,image_format);
    }
    else if(image_format==vizpet::PetDetector::ImageFormat::NV21){
      //This is done so that image orientation is resized and corrected before its passed to tagger
      #ifdef ANDROID_ARM_NEON
            image = vizpet_arm_neon::nv21_resize_convert_rotate(image_buffer, width, height, target_width, target_height, image_rotation);
        #else
            image = nv_convert_resize_rotate(image_buffer,width,height,target_width,target_height,image_rotation,image_format);
        #endif
    }
    else{
      //rest of the image types
      image = get_bgr_mat_from_buffer(image_buffer, width, height, image_format,target_width,target_height);
      image = rotate_image(image, image_rotation);
    }

    #ifdef DUMPBOX1 //Input image with detector boxes ; need to keep tese dumps on always
            {
              //cv::Mat dump_image;
             // dump_image = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, target_width, target_height, image_rotation);
              LG << "dump original boxes";
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_original_Post_dump/%d.jpg", original_image_post_cnt++);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                imwrite(buffer, image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                    
            }
            #endif //DUMPBOX
    //auto preprocess_finish = CURRENT_TIME;
    //LG<<"VZ Debug: time taken: pre-processing image:  "<< preprocess_finish - start_time <<" ms";
    std::vector<vizpet::BoundingBox> boxes;
     
    //auto engine_start = CURRENT_TIME;
    /*if(m_frame_number%60 == 0){
      homography_analyzer->clear();
      m_frame_number= 0;
    }*/
    /*if(check_image_continuity(image)){
      LOGV("Image Continuity is true returning the previous data");
      return true;
    }else{
      LOGV("Image Continuity is false Calling detector");
      #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump original boxes";
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_original_to_engine/%d.jpg", original_image_cnt++);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                imwrite(buffer, image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                    
            }
            #endif //DUMPBOX
      /*auto image_stabilize_start_time = CURRENT_TIME;
      cv::Mat stabilize_image = stabilize_for_illumination(image);
      stabilize_image = stabilize_for_focus(image,1.0,1.0);
      LOGV("VZ Debug: Image Stabilization: time taken: [%d ms]", CURRENT_TIME - image_stabilize_start_time);*/
      boxes = tagger->get_boxes(image);
      for(auto &hooks : tagger->bounding_box_hooks){
        boxes=hooks(boxes,{});
      }
      cv::Mat dumpImage;
      if(!boxes.empty()){
        std::lock_guard<std::mutex> guard_extraction(this->result_extraction_mtx);
        result_boxes = boxes;
        #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector";
              wo_resize_detect_img_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect_wo_correction/%d.jpg", wo_resize_detect_img_cnt);
                sprintf(buffer, "/sdcard/uod_det_wo_corr/%d.jpg", wo_resize_detect_img_cnt);
                rectangle(image, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(255,0,0), 3);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(image,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0),3);
                imwrite(buffer, image);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
              }      
            }
            #endif //DUMPBOX    
        correct_roi_boxes(target_width,target_height,image_rotation,width,height);
        #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            
              dumpImage = vizpet_arm_neon::nv21_resize_convert_without_rotate(image_buffer, width, height, width, height, image_rotation);
            {
              LG << "dump detector boxes";
              detect_image_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                if(result_boxes[i].tag_id == 4 || result_boxes[i].tag_id == 5){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect/%d.jpg", detect_image_cnt);
                sprintf(buffer, "/sdcard/uod_det_corr/%d.jpg", detect_image_cnt);
                rectangle(dumpImage, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(0,0,255), 3);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(dumpImage,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255),3);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                }
              }      
            }
            #endif //DUMPBOX
      }
      
      if(!boxes.size()){
        LG<<"No boxes are detected for tracking";
        result_boxes.clear();
        //reset_with_tracking();
        return false;
      }
      std::vector<vizpet::BoundingBox> box_info = getresultboxes();
      std::vector<vizpet::BoundingBox> pet_detect_boxes;
      #ifdef TEMPAVG
        #ifdef DUMPBOX
          LOGV("VZ Debug: # of boxes detected from detector execute_with_tracking: [%d]", box_info.size());
        #endif
        std::vector<vizpet::BoundingBox> temp_result_box_info ;
        if(!execute_ocsort_tracker){
          const int frame_rate = 30; const int track_buffer = 30;
          const float track_thresh = 0.5; const float high_thresh = 0.6;
          const float match_thresh = 0.8;
          const float det_thresh = 0.5; const int delta_t = 3;
          execute_ocsort_tracker = new ocsort::OCSort(det_thresh, track_buffer, 3, track_thresh, delta_t, "iou", 0.2, false);
          #ifdef DUMPBOX
            LOGV("VZ Debug: OCSortTracker Intialization is done");
          #endif
        //movingAverageFrames=1;
        if(movingAvgVec.size()){
        movingAvgVec.clear();
        }
      }
      
      // Convert boxes to Eigen matrix format for OCSort
      Eigen::MatrixXf dets(box_info.size(), 5);
      Eigen::Vector2f img_info(target_width, target_height);
      Eigen::Vector2f img_size(target_width, target_height);
      
      for(size_t i = 0; i < box_info.size(); i++){
        // Fill detection matrix: [x1, y1, x2, y2, score]
        dets(i, 0) = box_info[i].location.left;
        dets(i, 1) = box_info[i].location.top;
        dets(i, 2) = box_info[i].location.right;
        dets(i, 3) = box_info[i].location.bottom;
        dets(i, 4) = box_info[i].score;
      }
      
      int curr_frame_pet_boxes = 0;
      if(pet_detect_boxes.size()){
        pet_detect_boxes.clear();
      }
      // rect in in x, y, w, h format
      for(int i=0; i<box_info.size(); i++){
        if(box_info[i].tag_id == PET_ID){ //Pet Detector case Model class 8 -> PetFace , 11 -> PET
        #ifdef DUMPBOX
          LOGV("VZ Debug Det Label execute_with_tracking:[%s], Det Coord:left:[%d],top:[%d],right:[%d],bottom:[%d], score:[%f]", \
          box_info[i].tag.c_str(), box_info[i].location.left, box_info[i].location.top, box_info[i].location.right, box_info[i].location.bottom, box_info[i].score);
        #endif

          pet_detect_boxes.push_back(box_info[i]);
          curr_frame_pet_boxes++;
          //temp_result_box_info.push_back(box_info[i]);
          }else{
          temp_result_box_info.push_back(box_info[i]);
          }
      }
      #ifdef DUMPBOX
        LOGV("prev_frame_pet_boxes : [%d], curr_frame_pet_boxes : [%d]",prev_frame_pet_boxes,curr_frame_pet_boxes);
      #endif
      /*if(check_image_continuity(image) && prev_frame_pet_boxes == curr_frame_pet_boxes){
        LOGV("Image Continuity is true prev_frame_pet_boxes : [%d], curr_frame_pet_boxes : [%d]",prev_frame_pet_boxes,curr_frame_pet_boxes);
        result_boxes = prev_frame_result_boxes;
        return true;
      }
      
      prev_frame_pet_boxes = curr_frame_pet_boxes;*/
      // get tracks from OCSort tracker and just use the activated ones
      //prev_frame_result_boxes.clear();
      result_boxes.clear();
      /*vizpet::Rectangle maxIOUBox;
      maxIOUBox = vizpet::Rectangle(0,0,0,0);
      int maxIOU = INT_MIN;
      int trackidwithMaxIOU = -1;*/
      // get tracks from OCSort tracker
      auto tracks_new = execute_ocsort_tracker->update(dets, img_info, img_size);
      #ifdef DUMPBOX
        LOGV("VZ Debug: #tracks size execute : [%lu]" , tracks_new.size());
      #endif
      std::string pets("pets");
      std::string petface("petface");
      #ifdef DUMPBOX
        bool increament_detect_count= true;
        bool increament_tmpavg_track_detect_count = true;
        bool increament_detect_track_count = true;
      #endif

      //outfile << m_frame_number;
      movingAvg tempTrackedBox;
      std::set<int> tracked_box_indices; // track which box_info entries are covered by OCSort
      for (const auto& strack : tracks_new) {
        // strack format from OCSort: [x1, y1, x2, y2, track_id]
        if (strack.size() >= 5) {
          // Extract tracking information from the vector
          float x1 = strack(0);
          float y1 = strack(1);
          float x2 = strack(2);
          float y2 = strack(3);
          int track_id = static_cast<int>(strack(4));

          // Match tracked box back to original detections to recover score and class_id
          float best_dist = std::numeric_limits<float>::max();
          int best_idx = 0;
          for (size_t j = 0; j < box_info.size(); j++) {
            float dx1 = x1 - box_info[j].location.left;
            float dy1 = y1 - box_info[j].location.top;
            float dx2 = x2 - box_info[j].location.right;
            float dy2 = y2 - box_info[j].location.bottom;
            float dist = dx1*dx1 + dy1*dy1 + dx2*dx2 + dy2*dy2;
            if (dist < best_dist) {
              best_dist = dist;
              best_idx = j;
            }
          }
          float score = box_info[best_idx].score;
          int class_id = box_info[best_idx].tag_id;

          // Create a rectangle from the tracking information
          cv::Rect rect(x1, y1, x2 - x1, y2 - y1);
          
          #ifdef DUMPBOX
            LOGV("VZ Debug: Tracking result execute_with_tracking : #%d,%f,%d,%d,%d,%d,%d", track_id, score, (int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height), class_id);
          #endif
          
          #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector_tracker boxes";
              if(increament_detect_count){
                track_detect_image_cnt++;
                increament_detect_count = false;
              }
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect_track/%d.jpg", track_detect_image_cnt);
                sprintf(buffer, "/sdcard/uod_detect_track/%d.jpg", track_detect_image_cnt);
                rectangle(dumpImage, cv::Point((int)rect.x,(int)rect.y), cv::Point((int)(rect.x + rect.width),(int)(rect.y + rect.height)), cv::Scalar(0,255,0), 3);
                std::string detectorStr = std::to_string(class_id).append("_").append(std::to_string(score));
                putText(dumpImage,detectorStr,cv::Point((int)rect.x, (int)rect.y), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,255,0),3);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                   
            }
            #endif //DUMPBOX
             double maxIoU = 0.0;
             vizpet::BoundingBox bestMatch;
             int bestMatchIndex = -1;

            vizpet::Rectangle trackerBox = vizpet::Rectangle((int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height));
            vizpet::BoundingBox detector_box;
            // Match against all detections (not just pet_detect_boxes) so all classes get tracked
            for (size_t i = 0; i < box_info.size(); ++i) {
              const vizpet::BoundingBox& detectorBox = box_info[i];
              double iou = calculateIoU(trackerBox, detectorBox.location);
              if (iou > maxIoU) {
                maxIoU = iou;
                bestMatch = detectorBox;
                bestMatchIndex = i;
              }
            }
            if (bestMatchIndex != -1) {
              detector_box = bestMatch;
              tracked_box_indices.insert(bestMatchIndex);
            }
            
          vizpet::Rectangle avgResultBox;
          int index = 0;
          if(movingAvgVec.size()){
            auto low = std::lower_bound(movingAvgVec.begin(), movingAvgVec.end(), track_id , movingAvg::less_than());
            //LOGV("VZ Debug: Keeptracking low: [%d]",static_cast<int>(low));
            if(low >= movingAvgVec.begin() && low < movingAvgVec.end()){
              index = (low-movingAvgVec.begin());
              #ifdef DUMPBOX
                LOGV("VZ Debug: execute_with_tracking: index incase of track id present: [%d], trackid : [%d], movingAvgVec Size : [%d]",index, movingAvgVec[index].trackid, movingAvgVec.size());
              #endif
              vizpet::Rectangle trackedBox = vizpet::Rectangle((int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height));
              /*int prev_box_area = RectangleUtils::area_rectangle(movingAvgVec[index].prevOriginalBox);
              int curr_box_area = RectangleUtils::area_rectangle(trackedBox);
              float box_area_ratio =0;
              if(curr_box_area > prev_box_area){
                box_area_ratio = (float)(curr_box_area)/(float)(prev_box_area);
              }else{
                box_area_ratio = (float)(prev_box_area)/(float)(curr_box_area);
              }
              LOGV("VZ Debug: execute_with_tracking: prev_box_area : [%d] curr_box_area : [%d] box_area_ratio : [%f]",prev_box_area,curr_box_area,box_area_ratio);
              if(box_area_ratio > 1.3){
                trackedBox = movingAvgVec[index].prevOriginalBox;
              }*/
              float iou = get_overlap_between_boxes(movingAvgVec[index].smoothed_box,detector_box.location);
              iou = (iou < 0.0f) ? -(iou): iou;
              //LOGV("VZ Debug keeptracking: iou of tracked box with old tracked box for setting computing avg [%f]",iou);
              int interSectionArea = iou*100;
              #ifdef DUMPBOX
                LOGV("VZ Debug execute: interSectionArea : [%d], trackid : [%d]",interSectionArea,movingAvgVec[index].trackid);
              #endif
              if(interSectionArea < 80){
                movingAvgVec[index].smoothed_box = detector_box.location;
              }else{
                movingAvgVec[index].smoothed_box = update_weighted_smoother_box(detector_box.location,movingAvgVec[index].smoothed_box);
              }
              avgResultBox = movingAvgVec[index].smoothed_box;
              /*if(box_area_ratio <= 1.3){
                movingAvgVec[index].prevOriginalBox ={(int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height)};
              }*/
            }else{
              movingAvg tempMovingAvg;
              auto low = movingAvgVec.end();
              index = (low-movingAvgVec.begin());
              tempMovingAvg.counter = 1;
              tempMovingAvg.trackid = track_id;
              movingAvgVec.emplace_back(tempMovingAvg);
              #ifdef DUMPBOX
                LOGV("VZ Debug: execute_with_tracking: index incase of track id not present: [%d], trackid : [%d], movingAvgVec Size : [%d]",index, movingAvgVec[index].trackid, movingAvgVec.size());
              #endif
              //avgResultBox = ComputeMovingAverage(index, (int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height), movingAvgVec[index].trackid);
              avgResultBox = detector_box.location;
              movingAvgVec[index].smoothed_box =detector_box.location;
            }
              
          }else{
            movingAvg tempMovingAvg;
            tempMovingAvg.counter = 1;
            tempMovingAvg.trackid = track_id;
            movingAvgVec.emplace_back(tempMovingAvg);
            auto low = movingAvgVec.end();
            index = (low-movingAvgVec.begin())-1;
            #ifdef DUMPBOX
              LOGV("VZ Debug: execute_with_tracking: index incase of vector empty: [%d], trackid : [%d], movingAvgVec Size : [%d]",index, movingAvgVec[index].trackid, movingAvgVec.size());
            #endif
            //avgResultBox = ComputeMovingAverage(index, (int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height), movingAvgVec[index].trackid);
            avgResultBox = detector_box.location;
            movingAvgVec[index].smoothed_box =detector_box.location;
          }

          #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector_tracker boxes";
              if(increament_detect_track_count){
                track_detect_track_image_cnt++;
                increament_detect_track_count = false;
              }
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect_track/%d.jpg", track_detect_image_cnt);
                sprintf(buffer, "/sdcard/uod_detect_track_smooth/%d.jpg", track_detect_track_image_cnt);
                rectangle(dumpImage, cv::Point(avgResultBox.left,avgResultBox.top), cv::Point(avgResultBox.right,avgResultBox.bottom), cv::Scalar(255,0,0), 3);
                std::string detectorStr = std::to_string(track_id).append("_").append(std::to_string(class_id).append("_").append(std::to_string(score)));
                putText(dumpImage,detectorStr,cv::Point(avgResultBox.left,avgResultBox.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0),3);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                   
            }
            #endif //DUMPBOX
          //LOGV("Not applying TempAVG");
          //avgResultBox = vizpet::Rectangle((int)rect.x, (int)rect.y, (int)(rect.x + rect.width), (int)(rect.y + rect.height));
          vizpet::BoundingBox new_box({avgResultBox.left, avgResultBox.top, avgResultBox.right, avgResultBox.bottom},
          {width, height},
          (class_id == 3) ? petface : (class_id == 5) ? pets : std::string(""),
          score,
          class_id,
          track_id);
          //LOGV("VZ Debug: execute_with_tracking at Adding new Box result_boxes trackid : [%d], class: [%s], score: [%f], left : [%d], top : [%d], right : [%d], bottom : [%d]", new_box.trackid,(char*) new_box.tag.c_str(), new_box.score, new_box.location.left,new_box.location.top,new_box.location.right,new_box.location.bottom);
          result_boxes.emplace_back(new_box);
          if(m_frame_number==1){
            prev_frame_pet_result_boxes.emplace_back(new_box);
          }
          //prev_frame_result_boxes.emplace_back(new_box);
          /*for(int i =0;i<result_boxes.size();i++){
            LOGV("VZ Debug: execute_with_tracking at Adding result_boxes trackid : [%d], class: [%s], score: [%f], left : [%d], top : [%d], right : [%d], bottom : [%d]", result_boxes[i].trackid,(char*) result_boxes[i].tag.c_str(), result_boxes[i].score, result_boxes[i].location.left,result_boxes[i].location.top,result_boxes[i].location.right,result_boxes[i].location.bottom);
          }*/
        }

      }
      /*LOGV("VZ Debug: VZ Debug Before merging results box size : [%d]",result_boxes.size());
      for(int i =0; i<result_boxes.size();i++){
        LOGV("VZ Debug: VZ Debug Before merging results box tag : [%d] , class :[%s], score :[%f]", result_boxes[i].tag_id,result_boxes[i].tag.c_str(), result_boxes[i].score);
      }*/
      removeNestedBoxes(result_boxes);
      /*LOGV("VZ Debug: VZ Debug after merging results box size : [%d]",result_boxes.size());
      for(int i =0; i<result_boxes.size();i++){
        LOGV("VZ Debug: VZ Debug after merging results box tag : [%d] , class :[%s], score :[%f]", result_boxes[i].tag_id,result_boxes[i].tag.c_str(), result_boxes[i].score);
      }*/

      if(m_frame_number != 1){
        for(int i =0; i< result_boxes.size();i++){
          for(int j=0; j< prev_frame_pet_result_boxes.size();j++){
            if(result_boxes[i].trackid == prev_frame_pet_result_boxes[j].trackid){
              float prev_box_area = (float)prev_frame_pet_result_boxes[j].location.area();
              float result_box_area = (float)result_boxes[i].location.area();
              int area_percentage;
              if(prev_box_area > result_box_area){
                area_percentage = (result_box_area/prev_box_area)*100;
              }else{
                area_percentage = (prev_box_area/result_box_area)*100;
              }
              if(area_percentage < 90){
                float iou = get_overlap_between_boxes(prev_frame_pet_result_boxes[j].location,result_boxes[i].location);
                iou = (iou < 0.0f) ? -(iou): iou;
                int interSectionArea = iou*100;
                if(interSectionArea > 80){
                  result_boxes[i].location = prev_frame_pet_result_boxes[j].location;
                }
              }
              break;
            }
          }

        }
      }
      if(result_boxes.size() == prev_frame_pet_result_boxes.size()){
        for(int i = 0;i<result_boxes.size();i++){
          if(result_boxes[i].trackid == prev_frame_pet_result_boxes[i].trackid){
            if(result_boxes[i].score < prev_frame_pet_result_boxes[i].score){
              result_boxes[i].score = prev_frame_pet_result_boxes[i].score;
            }
          }
        }
      }else{
        prev_frame_pet_result_boxes.clear();
        for(int i = 0;i<result_boxes.size();i++){
           prev_frame_pet_result_boxes.emplace_back(result_boxes[i]);
        }
      }
      // merging the pet boxes
     /* result_boxes = adptiveMergeBoundingBoxes(result_boxes);
      #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector_tracker boxes";
              merge_track_detect_image_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect_track_merge/%d.jpg", merge_track_detect_image_cnt);
                sprintf(buffer, "/sdcard/uod_detect_track_merge/%d.jpg", merge_track_detect_image_cnt);
                rectangle(dumpImage, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(255,255,0), 1);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(dumpImage,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,0),1);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
              }      
            }
            #endif //DUMPBOX*/
      
      // Only OCSort-confirmed tracks are in result_boxes — no untracked detections added
      // This ensures all output boxes have persistent OCSort track IDs
      #ifdef DUMPBOX1 //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector_tracker boxes";
              if(increament_tmpavg_track_detect_count){
                tmpavg_track_detect_image_cnt++;
                increament_tmpavg_track_detect_count = false;
              }
              for(int i=0; i<result_boxes.size(); i++){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                sprintf(buffer, "/data/vendor/camera/uod_detect_track_tmpavg/%d.jpg", tmpavg_track_detect_image_cnt);
                //sprintf(buffer, "/sdcard/camera5/%d.jpg", detectorImageCounter);
                rectangle(dumpImage, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(255,0,0), 3);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(dumpImage,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,0,0),3);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
              }      
            }
            #endif //DUMPBOX
      #ifdef DUMPBOX //Input image with detector boxes ; need to keep tese dumps on always
            {
              LG << "dump detector_tracker boxes";
              merge_track_detect_image_cnt++;
              for(int i=0; i<result_boxes.size(); i++){
                if(result_boxes[i].tag_id == 4 || result_boxes[i].tag_id == 5){
                //cv::resize(image,image,cv::Size(target_width,target_height));
                char buffer[100];
                //sprintf(buffer, "/data/vendor/camera/uod_detect_track_merge/%d.jpg", merge_track_detect_image_cnt);
                sprintf(buffer, "/sdcard/uod_detect_track_merge/%d.jpg", merge_track_detect_image_cnt);
                rectangle(dumpImage, cv::Point(result_boxes[i].location.left,result_boxes[i].location.top), cv::Point(result_boxes[i].location.right,result_boxes[i].location.bottom), cv::Scalar(255,255,0), 1);
                std::string detectorStr = std::to_string(result_boxes[i].tag_id).append("_").append(std::to_string(result_boxes[i].score));
                putText(dumpImage,detectorStr,cv::Point(result_boxes[i].location.left, result_boxes[i].location.top), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,0),1);
                imwrite(buffer, dumpImage);
                if(-1 == chmod(buffer,0777)){
                LG << "no chmod";
                }
                }
              }      
            }
            #endif //DUMPBOX
      #endif
    //}

    generate_association_ids(image_buffer, result_boxes, width,height);

	  if(!result_boxes.empty())
		  sort(result_boxes.begin(),result_boxes.end(), &confidenceComparator);
    /*for(int i =0;i<result_boxes.size();i++){
      LOGV("VZ Debug: execute_with_tracking result_boxes trackid : [%d], class: [%s], score: [%f], left : [%d], top : [%d], right : [%d], bottom : [%d]", result_boxes[i].trackid,(char*) result_boxes[i].tag.c_str(), result_boxes[i].score, result_boxes[i].location.left,result_boxes[i].location.top,result_boxes[i].location.right,result_boxes[i].location.bottom);
    }*/

    auto execute_finish = CURRENT_TIME;
    LG<<"VZ Debug: pdt Total execute_with_tracking: time taken: "<<execute_finish - start_time <<" ms";
    return true;
}

/**
 * @brief Interface to bet the ROI bixes
 * @details extracts all the boxes post detection
 *
 * @param[in] none
 *
 * @return number of boxes
 */
int  PDTImpl::get_n_boxes(){
    // LOGV("get_n_boxes: return 1");
    // return 1; 
    if(!result_boxes.empty()){
        return result_boxes.size();
      }
    return 0;
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
bool PDTImpl::get_box_info(BoxInfo box_infos[], int n){
    std::lock_guard<std::mutex> guard_extraction(this->result_extraction_mtx);
    // LOGV("get_box_info: return 1");
      // box_infos[0].trackid        = 1;
      // box_infos[0].score          = 0.90;

      // box_infos[0].left           = 275;
      // box_infos[0].top            = 210;
      // box_infos[0].right          = 1080;
      // box_infos[0].bottom         = 1085;
      // box_infos[0].image_width    = 512;
      // box_infos[0].image_height   = 512;

      // box_infos[0].tag_class      = vizpet::PetDetector::TagCategory::TAG_PET; //Pet Detector case Camera Module  PET -> 10
      // box_infos[0].tag            = "pets";
      // LOGV("VZ Debug: pdt get_box_info detected trackid : [%d],class: [%s], score: [%f], left : [%d], top : [%d], bottom : [%d], right : [%d]", box_infos[0].trackid, box_infos[0].tag, box_infos[0].score, box_infos[0].left,box_infos[0].top,box_infos[0].right,box_infos[0].bottom);
      

    if(tagger == nullptr) return false; 
      if(result_boxes.empty()){
        return false;
      }
      
      if(n > this->get_n_boxes()){
        n = this->get_n_boxes();
      }
      
      #ifdef DUMPBOX
        LOGV("VZ Debug: get_box_info num boxes: [%d]", n);
      #endif
      for(int i=0; i<n; i++){
        // LOGV("VZ Debug: pdt get_box_info detected i=%d",i);
        box_infos[i].score          = result_boxes[i].score;
        
        // return true;
        box_infos[i].trackid        = result_boxes[i].trackid;
        // box_infos[i].trackid = 1;
        
        box_infos[i].left           = result_boxes[i].location.left;
        box_infos[i].top            = result_boxes[i].location.top;
        box_infos[i].right          = result_boxes[i].location.right;
        box_infos[i].bottom         = result_boxes[i].location.bottom;
        box_infos[i].image_width    = result_boxes[i].image_size.width;
        box_infos[i].image_height   = result_boxes[i].image_size.height;

        if(result_boxes[i].tag_id == 5){
          box_infos[i].tag_class      = vizpet::PetDetector::TagCategory::TAG_PET; //Pet Detector case Camera Module  PET -> 10
          box_infos[i].tag            = "pets";
        }
        else if(result_boxes[i].tag_id == 3){
          box_infos[i].tag_class      = vizpet::PetDetector::TagCategory::TAG_PETFACE; //Pet Detector case Camera Module  TAG_PETFACE -> 8
          box_infos[i].tag            = "petface";
        }
        else if(result_boxes[i].tag_id == 6){
          box_infos[i].tag_class      = vizpet::PetDetector::TagCategory::TAG_PET_EYES; //Pet Detector case Camera Module  TAG_PETFACE -> 8
          box_infos[i].tag            = "peteyes";
        }
        else{
          box_infos[i].tag_class      = (vizpet::PetDetector::TagCategory)(result_boxes[i].tag_id);
          box_infos[i].tag            = (char*) result_boxes[i].tag.c_str();
        }
          LG << "VZ Debug: pdt get_box_info detected trackid : "<<box_infos[i].trackid<<", class id : "<<result_boxes[i].tag_id<<", score: "<<box_infos[i].score<<", left : "<<box_infos[i].left<<", top : "<<box_infos[i].top<<", right : "<<box_infos[i].right<<", bottom : "<<box_infos[i].bottom;
      }
      return true;
}

/**
 * @brief rotation_corrected_roi corrects the ROI based on rotation
 * @details Corrects the roi based on roi and image dimensions
 *
 * @param[in] defines the roi for the image
 * @param[in] target width of image
 * @param[in] target height of image   
 * @param[in] image_rotation defines rotation angle of image
 *
 * @return roi with correction
 */
vizpet::Rectangle rotation_corrected_roi(vizpet::Rectangle roi,int width,int height,const int angular_clockwise_rotation){
  vizpet::Rectangle bounds({roi.left, roi.top,
                roi.right, roi.bottom});
  int image_width  = width;
  int image_height = height;
  switch(angular_clockwise_rotation){
    case 270:
      bounds = {bounds.top, image_height - bounds.right, bounds.bottom, image_height - bounds.left};
      break;
    case 180:
      bounds = {image_width - bounds.right, image_height - bounds.bottom, image_width - bounds.left, image_height - bounds.top};
      break;
    case 90:
      bounds = {image_width - bounds.bottom, bounds.left, image_width - bounds.top, bounds.right};
      break;
  }
  return bounds;
}

/**
 * @brief correct_roi_boxes corrects the ROI based on rotation
 * @details Corrects the roi based on new image dimensions
 *
 * @param[in] target width of image
 * @param[in] target height of image   
 * @param[in] image_rotation defines rotation angle of image
 * @param[in] width of image
 * @param[in] height of image
 *
 * @return void
 */
void PDTImpl::correct_roi_boxes(int target_width,int target_height,const int angular_rotation_clockwise,int width,int height){
  for(int i=0;i<result_boxes.size();i++){
    vizpet::Rectangle region=rotation_corrected_roi(vizpet::Rectangle(result_boxes[i].location.left,
                                            result_boxes[i].location.top,
                                            result_boxes[i].location.right,
                                            result_boxes[i].location.bottom),
                                            target_width,target_height,angular_rotation_clockwise);


    result_boxes[i].location.left=(int)(((float)region.left)*(((float)width)/((float)target_width)));
    result_boxes[i].location.top=(int)(((float)region.top)*(((float)height)/((float)target_height)));
    result_boxes[i].location.right=(int)(((float)region.right)*(((float)width)/((float)target_width)));
    result_boxes[i].location.bottom=(int)(((float)region.bottom)*(((float)height)/((float)target_height)));
    // result_boxes[i].image_size.width=region.right-region.left+1;
    // result_boxes[i].image_size.height=region.bottom-region.top+1; 
  }
}

// Execute with Tracking

bool PDTImpl::inittracking(){
    //LG<<"VZ Debug PDTImpl ::init_tracking ";
    const int frame_rate = 30; const int track_buffer = 30;
    const float track_thresh = 0.5; const float high_thresh = 0.6;
    const float match_thresh = 0.8;
    const float det_thresh = 0.5; const int delta_t = 3;
    ocsort_tracker = new ocsort::OCSort(det_thresh, track_buffer, 3, track_thresh, delta_t, "iou", 0.2, false);
    m_frame_number = 0;
    return true;
}

void PDTImpl::deinittracking(){
    LG<<"VZ Debug PDTImpl::deinit_tracking ";
    if(ocsort_tracker){
      delete ocsort_tracker;
      ocsort_tracker = 0;
    }
}

bool PDTImpl::execute_starttracking(char* image_buffer, 
                int width, int height, 
                int image_rotation, int orientation,
                ImageFormat image_format, 
                int touch_x, int touch_y, trkRes& outroi_info){
    inittracking();
    #ifdef DUMPBOX 
      LOGV("VZ Debug PDTImpl::execute_starttracking, width=[%d], height=[%d]", width, height);
    #endif
    trackingstopped = false;
    //m_frame_number = 0;
    m_orientation = orientation;
    m_imgWidth = width;
    m_imgheight = height;
    m_rotation = image_rotation;
    m_image_format = image_format;
    m_touch_x = -1;
    m_touch_y = -1;

    outroi_info.numOfBoxes = 0;

    bool execute_status = execute(image_buffer, width, height, image_rotation, image_format);
    // bool execute_status = object_detector->execute_on_touch(image_buffer, image.cols, image.rows, 0, vizpet::LightObjectDetector::ImageFormat::BGR, 100, 300);
    #ifdef DUMPBOX
      LOGV("VZ Debug execute_starttracking Execute status: =[%s]", execute_status ? "true": "failed");
    #endif
    if(!execute_status){
      #ifdef DUMPBOX
        LOGV("VZ Debug Execution of detection failed\n");
      #endif
        return false;
    }
    //int n_boxes = m_object_detector->get_n_boxes();
    int n_boxes = get_n_boxes();
    //vizpet::LightObjectDetector::BoxInfo box_infos[n_boxes]; 

    std::vector<vizpet::BoundingBox> box_info = getresultboxes();
    if(!box_info.size()){
      #ifdef DUMPBOX
        LOGV("VZ Debug No boxes are detected for tracking");
      #endif
        return false;
    }
    #ifdef DUMPBOX
      LOGV("VZ Debug: # of boxes detected from detector: [%d]", n_boxes);
    #endif
    auto start_time = CURRENT_TIME;
    
    // Convert boxes to Eigen matrix format for OCSort
    Eigen::MatrixXf dets(n_boxes, 5);
    Eigen::Vector2f img_info(width, height);
    Eigen::Vector2f img_size(width, height);
    
    for(int i=0; i<n_boxes; i++){
      #ifdef DUMPBOX
        LOGV("VZ Debug Det Label:[%s], Det Coord:left:[%d],top:[%d],right:[%d],bottom:[%d], score:[%f]", \
          box_info[i].tag.c_str(), box_info[i].location.left, box_info[i].location.top, box_info[i].location.right, box_info[i].location.bottom, box_info[i].score);
        #endif

        // Fill detection matrix: [x1, y1, x2, y2, score]
        dets(i, 0) = box_info[i].location.left;
        dets(i, 1) = box_info[i].location.top;
        dets(i, 2) = box_info[i].location.right;
        dets(i, 3) = box_info[i].location.bottom;
        dets(i, 4) = box_info[i].score;
    }

    // get tracks from OCSort tracker
    auto tracks = ocsort_tracker->update(dets, img_info, img_size);
    #ifdef DUMPBOX
      LOGV("VZ Debug: #tracks size : [%lu]" , tracks.size());
    #endif
    
    // Convert tracks to output format
    for (const auto& track : tracks) {
        // track format from OCSort: [x1, y1, x2, y2, track_id]
        if (track.size() >= 5) {
            int track_id = static_cast<int>(track(4));
            float tx1 = track(0), ty1 = track(1), tx2 = track(2), ty2 = track(3);

            // Match tracked box back to original detections to recover score and class_id
            float best_dist = std::numeric_limits<float>::max();
            int best_idx = 0;
            for (int j = 0; j < n_boxes; j++) {
              float dx1 = tx1 - box_info[j].location.left;
              float dy1 = ty1 - box_info[j].location.top;
              float dx2 = tx2 - box_info[j].location.right;
              float dy2 = ty2 - box_info[j].location.bottom;
              float dist = dx1*dx1 + dy1*dy1 + dx2*dx2 + dy2*dy2;
              if (dist < best_dist) {
                best_dist = dist;
                best_idx = j;
              }
            }
            float score = box_info[best_idx].score;
            int class_id = box_info[best_idx].tag_id;

            // Convert class_id to tag_class
            if(class_id == 3) {  // PETFACE_ID
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = PetDetector::TagCategory::TAG_PETFACE;
            } else if(class_id == 5) {  // PET_ID
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = vizpet::PetDetector::TagCategory::TAG_PET;
            } else {
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = PetDetector::TagCategory::TAG_INVALID;
            }

            outroi_info.Box[outroi_info.numOfBoxes].ID = track_id;
            outroi_info.Box[outroi_info.numOfBoxes].score = score;
            outroi_info.Box[outroi_info.numOfBoxes].rect.left = static_cast<int>(track(0));
            outroi_info.Box[outroi_info.numOfBoxes].rect.top = static_cast<int>(track(1));
            outroi_info.Box[outroi_info.numOfBoxes].rect.right = static_cast<int>(track(2));
            outroi_info.Box[outroi_info.numOfBoxes].rect.bottom = static_cast<int>(track(3));
            outroi_info.numOfBoxes++;
        }
    }
    
    #ifdef TEMPAVG
    for(auto movingAvg : movingAvgVec){
      movingAvg.counter = 0;
      movingAvg.trackid = 0;
    }
    #endif
    m_frame_number += 1;
    trackingstarted = true;
    auto execute_finish = CURRENT_TIME;
    #ifdef DUMPBOX
      LOGV("VZ Debug: Total execute_starttracking: time taken: [%lld]ms", execute_finish - start_time);
    #endif
    return true;
}

bool PDTImpl::execute_keeptracking(char* image_buffer, int image_rotation, trkRes& outroi_info){
    if(!trackingstarted || trackingstopped)
        return false;
    #ifdef DUMPBOX
      LOGV("VZ Debug ObjectTrackerImpl::keeptracking");
    #endif
    outroi_info.numOfBoxes = 0;
    bool execute_status = execute(image_buffer, m_imgWidth, m_imgheight, m_rotation, m_image_format);
    #ifdef DUMPBOX
      LOGV("VZ Debug execute_keeptracking Execute status: =[%s]", execute_status ? "true": "failed");
    #endif
    if(!execute_status){
        LG<<"Execution of detection failed\n";
        return false;
    }
    //int n_boxes = m_object_detector->get_n_boxes();
    int n_boxes = get_n_boxes();
    //vizpet::LightObjectDetector::BoxInfo box_infos[n_boxes]; 

    std::vector<vizpet::BoundingBox> box_info = getresultboxes();
    if(!box_info.size()){
        LG<<"No boxes are detected for tracking";
        return false;
    }
    #ifdef DUMPBOX
      LOGV("VZ Debug: # of boxes detected from detector: [%d]", n_boxes);
    #endif
    auto start_time = CURRENT_TIME;
    
    // Convert boxes to Eigen matrix format for OCSort
    Eigen::MatrixXf dets(n_boxes, 5);
    Eigen::Vector2f img_info(m_imgWidth, m_imgheight);
    Eigen::Vector2f img_size(m_imgWidth, m_imgheight);
    
    for(int i=0; i<n_boxes; i++){
        // Fill detection matrix: [x1, y1, x2, y2, score]
        dets(i, 0) = box_info[i].location.left;
        dets(i, 1) = box_info[i].location.top;
        dets(i, 2) = box_info[i].location.right;
        dets(i, 3) = box_info[i].location.bottom;
        dets(i, 4) = box_info[i].score;
    }

    // get tracks from OCSort tracker
    auto tracks = ocsort_tracker->update(dets, img_info, img_size);
    #ifdef DUMPBOX
      LOGV("VZ Debug: #tracks size : [%lu]" , tracks.size());
    #endif
    
    // Convert tracks to output format
    for (const auto& track : tracks) {
        // track format from OCSort: [x1, y1, x2, y2, track_id]
        if (track.size() >= 5) {
            int track_id = static_cast<int>(track(4));
            float tx1 = track(0), ty1 = track(1), tx2 = track(2), ty2 = track(3);

            // Match tracked box back to original detections to recover score and class_id
            float best_dist = std::numeric_limits<float>::max();
            int best_idx = 0;
            for (int j = 0; j < n_boxes; j++) {
              float dx1 = tx1 - box_info[j].location.left;
              float dy1 = ty1 - box_info[j].location.top;
              float dx2 = tx2 - box_info[j].location.right;
              float dy2 = ty2 - box_info[j].location.bottom;
              float dist = dx1*dx1 + dy1*dy1 + dx2*dx2 + dy2*dy2;
              if (dist < best_dist) {
                best_dist = dist;
                best_idx = j;
              }
            }
            float score = box_info[best_idx].score;
            int class_id = box_info[best_idx].tag_id;

            // Convert class_id to tag_class
            if(class_id == 3) {  // PETFACE_ID
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = PetDetector::TagCategory::TAG_PETFACE;
            } else if(class_id == 5) {  // PET_ID
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = vizpet::PetDetector::TagCategory::TAG_PET;
            } else {
                outroi_info.Box[outroi_info.numOfBoxes].tag_class = PetDetector::TagCategory::TAG_INVALID;
            }

            outroi_info.Box[outroi_info.numOfBoxes].ID = track_id;
            outroi_info.Box[outroi_info.numOfBoxes].score = score;
            outroi_info.Box[outroi_info.numOfBoxes].rect.left = static_cast<int>(track(0));
            outroi_info.Box[outroi_info.numOfBoxes].rect.top = static_cast<int>(track(1));
            outroi_info.Box[outroi_info.numOfBoxes].rect.right = static_cast<int>(track(2));
            outroi_info.Box[outroi_info.numOfBoxes].rect.bottom = static_cast<int>(track(3));
            outroi_info.numOfBoxes++;
        }
    }

    m_frame_number += 1;
    auto execute_finish = CURRENT_TIME;
    #ifdef DUMPBOX
      LOGV("VZ Debug: Total keep_starttracking: time taken: [%lld]ms", execute_finish - start_time);
    #endif
    return true;
}

bool PDTImpl::stoptracking(){
    LG<<"VZ Debug ObjectTrackerImpl::stoptracking";
    trackingstarted = false;
    trackingstopped = true;
    return true;

}

void PDTImpl::resettracking(){
    LG<<"VZ Debug ObjectTrackerImpl::resettracking";
    trackingstarted = false;
    trackingstopped = true;
    if(ocsort_tracker){
      delete ocsort_tracker;
      ocsort_tracker = 0;
    }
      
    return;
}

void PDTImpl::reset_with_tracking(){
    LG<<"VZ Debug ObjectTrackerImpl::reset_with_tracking";
    #ifdef TEMPAVG
      movingAvgVec.clear();
    #endif
    m_frame_number = 0;
    //homography_analyzer->clear();
    /*if(execute_ocsort_tracker){
      delete execute_ocsort_tracker;
      execute_ocsort_tracker = 0;
    }*/
    return;
}

/*#ifdef TEMPAVG
vizpet::Rectangle PDTImpl::ComputeMovingAverage(int index, int left, int top, int right, int bottom,int trackid){
    vizpet::Rectangle resultBox;
    vizpet::Rectangle box ={left,top,right,bottom};
    int K = movingAvgVec[index].counter;
    left = std::max(0, left);
    top = std::max(0, top);
    right = std::min(m_width-1, right);
    bottom = std::min(m_height-1, bottom);
    LOGV("VZ Debug PDTImpl::ComputeMovingAverage left:[%d] top:[%d] right:[%d] bottom:[%d] K:[%d] index : [%d]",left,top,right,bottom,K,index);
    if(K==1){
        resultBox.left = left;
        resultBox.top = top;
        resultBox.right = right;
        resultBox.bottom = bottom;
    }else{
      int prev_box_width = movingAvgVec[index].prevOriginalBox.right - movingAvgVec[index].prevOriginalBox.left +1;
      int prev_box_height =  movingAvgVec[index].prevOriginalBox.bottom - movingAvgVec[index].prevOriginalBox.top +1;
      int current_box_width = right - left+1;
      int current_box_height = bottom - top+1;
      int tmp_avg_width = (prev_box_width+current_box_width)/2;
      int tmp_avg_height = (prev_box_height+current_box_height)/2; 
      int prev_box_center_x = (movingAvgVec[index].prevOriginalBox.right + movingAvgVec[index].prevOriginalBox.left)/2;
      int prev_box_center_y = (movingAvgVec[index].prevOriginalBox.bottom + movingAvgVec[index].prevOriginalBox.top)/2;
      int curr_box_center_x = (right+left)/2;
      int curr_box_center_y = (bottom+top)/2;
      int new_box_center_x = (prev_box_center_x + curr_box_center_x)/2;
      int new_box_center_y = (prev_box_center_y + curr_box_center_y)/2;

      int new_box_width_half = tmp_avg_width/2;
      int new_box_height_half = tmp_avg_height/2;
      resultBox.left = new_box_center_x - new_box_width_half;
      resultBox.top = new_box_center_y - new_box_height_half;
      resultBox.right = new_box_center_x + new_box_width_half;;
      resultBox.bottom = new_box_center_y + new_box_height_half;

      resultBox.left = std::max(0, resultBox.left);
      resultBox.top = std::max(0, resultBox.top);
      resultBox.right = std::min(m_width-1, resultBox.right);
      resultBox.bottom = std::min(m_height-1, resultBox.bottom);
    }
    auto execute_finish = CURRENT_TIME;
    //LOGV("VZ Debug: PDTImpl::ComputeMovingAverage: time taken (ms): [%d]", execute_finish - start_time);
    LOGV("VZ Debug: PDTImpl::ComputeMovingAverage resultBox.left:[%d] resultBox.top:[%d] resultBox.right:[%d] resultBox.bottom:[%d], trackid : [%d]",resultBox.left,resultBox.top,resultBox.right,resultBox.bottom, trackid);
    return resultBox;
}
#endif*/
