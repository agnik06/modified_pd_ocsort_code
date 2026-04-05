/**
 * @file   image_utils.cpp
 * @brief  This file implements different image utilities
 *
 * This file implements different image utilities
 */

#include "image_utils.hpp"
#include "logger/logger.hpp"

/**
 * @brief Checks if an image is dark or not.
 * If the image is totally dark models are not executed and it saves computation time.
 * 
 * @param[in] image
 *
 * @return bool true if dark, false otherwise
 */
bool is_image_dark(cv::Mat image){
    cv::Mat grey;
    cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);

    cv::Mat inRng;
    cv::inRange(grey, cv::Scalar(0, 0, 0), cv::Scalar(20, 20, 20), inRng);

    int zero_pixels = cv::countNonZero(inRng);
    int total_pixels = grey.total();
    float zero_pixels_fraction = float(zero_pixels) / total_pixels;

    cv::Scalar mean;
    cv::Scalar std;
    cv::meanStdDev(grey, mean, std);

    if (zero_pixels_fraction > 0.98 && std[0] < 20) {
        return true;
    }
  return false;
}


// void set_UV_mat(cv::Mat uv_orig,cv::Mat uv_result,int width,int )

char* halve_nv21_buffer(char* bitmap_buffer, const int image_width, int const image_height) {
    cv::Mat YUV(image_height / 2 + image_height / 4, image_width / 2, CV_8UC1, bitmap_buffer);
    char *yuv =(char*)YUV.data;
    // halve yuma
    int i = 0;
    for (int y = 0; y < image_height; y+=2) {
        for (int x = 0; x < image_width; x+=2) {
            yuv[i] = bitmap_buffer[y * image_width + x];
            i++;
        }
    }
    // halve U and V color components
    for (int y = 0; y < image_height / 2; y+=2) {
        for (int x = 0; x < image_width; x += 4) {
            yuv[i] = bitmap_buffer[(image_width * image_height) + (y * image_width) + x];
            i++;
            yuv[i] = bitmap_buffer[(image_width * image_height) + (y * image_width) + (x + 1)];
            i++;
        }
    }

    return yuv;
}

/**
 * @brief Get bgr mat from image buffer
 * 
 * @param[in] bitmap buffer of image
 * @param[in] width of image
 * @param[in] height of image
 * @param[in] Image format
 * @param[out] target image width
 * @param[out] target image height
 *
 * @return Converted image 
 */
cv::Mat get_bgr_mat_from_buffer(char* bitmap_buffer, int width, int height, const vizpet::PetDetector::ImageFormat format,int& target_width,int& target_height){
    cv::Mat image;
    if (format == vizpet::PetDetector::ImageFormat::NV21) {
        LG<<"VZ Debug: Converting image format - NV21";
        if(width*height>350000){
            LG<<"CD Debug:resizing:"<<width<<"  ddd="<<height;
            auto start_time=CURRENT_TIME;
            char *resized_buffer=halve_nv21_buffer(bitmap_buffer,width,height);
            LG<<"CD Debug: resize time:"<<CURRENT_TIME-start_time<<"ms";
            start_time=CURRENT_TIME;
            cv::Mat yuv_resized(height/2+height/4,width/2,CV_8UC1,resized_buffer);
            cv::Mat frame(height/2,width/2, CV_8UC3);
            cv::cvtColor(yuv_resized, frame, cv::COLOR_YUV2BGR_NV21);  // CV_YUV2BGR_NV21 = 93,CV_YUV2BGRA_NV21
            LG<<"CD Debug: convert time:"<<CURRENT_TIME-start_time<<"ms";
            image = frame;
        }
        else{
            cv::Mat yuv(height + height / 2, width, CV_8UC1, bitmap_buffer);
            cv::Mat frame(height,width, CV_8UC3);
            cv::cvtColor(yuv, frame, cv::COLOR_YUV2BGR_NV21);  // CV_YUV2BGR_NV21 = 93,CV_YUV2BGRA_NV21
            image = frame;
        }
        
    }
    else if (format == vizpet::PetDetector::ImageFormat::RGBA) {
        LG<<"VZ Debug: Converting image format - RGBA";
        cv::Mat rgba(height, width, CV_8UC4, bitmap_buffer);
        cv::Mat frame(height, width, CV_8UC3);
        cv::cvtColor(rgba, frame, cv::COLOR_RGBA2BGR);  // CV_RGBA2BGR = 3
        image = frame;
    }
    else {
        LG<<"VZ Debug: Converting image format - BGR";
        cv::Mat bgr(height, width, CV_8UC3, bitmap_buffer);
        image = bgr;
    }
    auto start_time=CURRENT_TIME;
    cv::resize(image,image,cv::Size(target_width,target_height));
    LG<<"VZ Debug: time in resize:"<<CURRENT_TIME-start_time;
    return image;
}

/**
 * @brief Rotate given image clockwise based on angle
 * 
 * @param[in] Image
 * @param[in] angle to rotate with
 *
 * @return Rotated image
 */
cv::Mat rotate_image(cv::Mat image, int clockwise_angle) {
    #ifdef DUMPBOX
        LOGV("rotate_image : clockwise_angle : [%d]",clockwise_angle);
    #endif
    int rotate_code;
    if (clockwise_angle == 90) {
        rotate_code = 2;
    } else if (clockwise_angle == 180) {
        rotate_code = 1;
    } else if (clockwise_angle == 270) {
        rotate_code = 0;
    } else {
        return image;
    }
    #ifdef DUMPBOX
        LOGV("rotate_image : clockwise_angle : [%d], rotate_code : [%d]",clockwise_angle, rotate_code);
    #endif
    cv::Mat rotated_image;
    cv::rotate(image, rotated_image, rotate_code);
    return rotated_image;
}

cv::Mat exif_correction(cv::Mat image, const int orientation) {
    cv::Mat corrected_image;
    switch (orientation) {
        case 1: {
            corrected_image = image;
            break;
        }
        case 2: {
            cv::Mat dst;
            cv::flip(image, dst, 1);
            corrected_image = dst;
            break;
        }
        case 3: {
            corrected_image = rotate_image(image, 180);
            break;
        }
        case 4: {
            cv::Mat dst;
            cv::flip(image, dst, 0);
            corrected_image = dst;
            break;
        }
        case 5: {
            cv::Mat dst;
            cv::transpose(image, dst);
            corrected_image = dst;
            break;
        }
        case 6: {
            corrected_image = rotate_image(image, 270);
            break;
        }
        case 7: {
            cv::Mat rotated_90 = rotate_image(image, 90);
            cv::Mat dst;
            cv::flip(rotated_90, dst, 1);
            corrected_image = dst;
            break;
        }
        case 8: {
            corrected_image = rotate_image(image, 90);
            break;
        }
        default: {
            corrected_image = image;
            break;
        }
    }
    return corrected_image;
}


#ifndef MAX
#define MAX(x, y) ((x) > (y) ? x : y)
#define MIN(x, y) ((x) < (y) ? x : y)
#endif

/**
 * @brief Get image rect with 4:3 center crop
 * @details 4:3 is 640x480 By default the image is reduced to this center crop if its > 4:3
 * its resized.
 * 
 * @param[in] Image
 *
 * @return rect with centercrop of 4:3
 */
cv::Rect get_4_3_center_crop_rect(cv::Mat image) {
    int max_dim = MAX(image.rows, image.cols);
    int min_dim = MIN(image.rows, image.cols);
    float aspect_ratio = float(max_dim) / min_dim;

    float target_aspect_ratio = 4.0f/3.0f; 
    cv::Rect crop_rect(0,0, image.cols, image.rows);

    if(aspect_ratio <= target_aspect_ratio){
        return crop_rect;
    }
    
    if(max_dim == image.cols){
        int crop_width = (int)(image.rows * target_aspect_ratio);
        crop_rect.x = (crop_rect.width - crop_width)/2;
        crop_rect.width = crop_width;
    }
    else{
        int crop_height = (int)(image.cols * target_aspect_ratio);
        crop_rect.y = (crop_rect.height - crop_height)/3; //Crop rect is shifted to top
        crop_rect.height = crop_height;
    }

    return crop_rect;
    // cv::Mat crop_image = image(crop_rect).clone();
    // return crop_image;
}

void fill_mask_buffer(std::string& buffer, cv::Mat mask){
    if(mask.empty()){
        LG<<"VZ Debug: Cannot fill mask buffer as given mask is empty.";
    }
    buffer.clear();
    for(int i=0; i < mask.rows; i++){
        for(int j=0; j < mask.cols; j++){
            uchar pixel_value = mask.at<uchar>(cv::Point(j,i));
            buffer.push_back((char)pixel_value);
        }
    }
}

cv::Vec3b get_bgr_pixel_yuv(char* yuv, const int image_width, int const image_height, int i, int j){
    const int frame_size = image_width * image_height;        
    int y = yuv[i * image_width + j];
    int v = yuv[frame_size + (int)(i/2) * image_width + 2 * (int)(j/2)];
    int u = yuv[frame_size + (int)(i/2) * image_width + 2 * (int)(j/2) + 1];

    float b = (1.164 * (y - 16) + 2.018 * (u - 128));
    float g = (1.164 * (y - 16) - 0.813 * (v - 128) - 0.391 * (u - 128));
    float r = (1.164 * (y - 16) + 1.596 * (v - 128));

    cv::Vec3b color;
    color[0] = b > 255.0f ? 255 : b < 0 ? 0 : char(b);
    color[1] = g > 255.0f ? 255 : g < 0 ? 0 : char(g);
    color[2] = r > 255.0f ? 255 : r < 0 ? 0 : char(r);
    return color;
}

cv::Vec3b get_bgr_pixel_yvu(char* yuv, const int image_width, int const image_height, int i, int j){
    const int frame_size = image_width * image_height;        
    int y = yuv[i * image_width + j];
    int u = yuv[frame_size + (int)(i/2) * image_width + 2 * (int)(j/2)];
    int v = yuv[frame_size + (int)(i/2) * image_width + 2 * (int)(j/2) + 1];

    float b = (1.164 * (y - 16) + 2.018 * (u - 128));
    float g = (1.164 * (y - 16) - 0.813 * (v - 128) - 0.391 * (u - 128));
    float r = (1.164 * (y - 16) + 1.596 * (v - 128));

    cv::Vec3b color;
    color[0] = b > 255.0f ? 255 : b < 0 ? 0 : char(b);
    color[1] = g > 255.0f ? 255 : g < 0 ? 0 : char(g);
    color[2] = r > 255.0f ? 255 : r < 0 ? 0 : char(r);
    return color;
}

/**
 * @brief Resize and rotate the given image
 * 
 * @param[in] Image buffer with yuv format
 * @param[in] input image width
 * @param[in] input image height
 * @param[in] converted image width
 * @param[in] converted image height
 * @param[in] rotated angle
 * @param[in] image format
 *
 * @return Rotated Image
 */
cv::Mat nv_convert_resize_rotate(char* yuv, const int image_width, 
                                    int const image_height, int const target_width,
                                    int const target_height, int rotation_angle,
                                    const vizpet::PetDetector::ImageFormat format) {
    float step_w = float(image_width) / target_width;
    float step_h = float(image_height) / target_height;
    
    auto *get_pixel = &get_bgr_pixel_yvu;
    if(format == vizpet::PetDetector::ImageFormat::NV21){
        get_pixel = &get_bgr_pixel_yuv;
    }
    if (rotation_angle == 270){
        cv::Mat bgr(target_height, target_width, CV_8UC3);

        for(int i=0; i<target_height; i++){
            for(int j=0; j<target_width; j++){
                bgr.at<cv::Vec3b>(i, j) = (*get_pixel)(yuv, image_width, image_height, (target_height-j-1)*step_h, i*step_w);
            }
        }
        return bgr;
    }
    else if (rotation_angle == 180){
        cv::Mat bgr(target_width, target_height, CV_8UC3);

        for(int i=0; i<target_height; i++){
            for(int j=0; j<target_width; j++){
                bgr.at<cv::Vec3b>(i, j) = (*get_pixel)(yuv, image_width, image_height, (target_height-i-1)*step_h, (target_width-j-1)*step_w);
            }
        }
        return bgr;
    }
    else if (rotation_angle == 90){
        cv::Mat bgr(target_width, target_height, CV_8UC3);

        for(int i=0; i<target_height; i++){
            for(int j=0; j<target_width; j++){
                bgr.at<cv::Vec3b>(i, j) = (*get_pixel)(yuv, image_width, image_height, j*step_h, (target_width-i-1)*step_w);
            }
        }
        return bgr;
    }
    else{
        cv::Mat bgr(target_width, target_height , CV_8UC3);
        for(int i=0; i<target_height; i++){
            for(int j=0; j<target_width; j++){
                bgr.at<cv::Vec3b>(i, j) = (*get_pixel)(yuv, image_width, image_height, i*step_h, j*step_w);
            }
        }
        if (rotation_angle == 0){
           return bgr;
        }
        else{
            LG<<"Unsupported rotation parameter";
        }
    }       
}

cv::Mat normalize_image(cv::Mat uchar_image, std::vector<float> means, std::vector<float> scales){
    cv::Mat image;
    LG << "normalize | image channels with means & scale image.channels() ="<<uchar_image.channels();
    uchar_image.convertTo(image, CV_32FC3);//CV_MAKE_TYPE(CV_32F, 6)); //means.size()));
    if(image.channels() != means.size() || image.channels() != scales.size()){
        LG << "Image not normalized | image channels doesnt match means & scale";
        LG << "Image not normalized | image channels = "<< image.channels()<<" doesnt match means.size()="<<means.size()<<" & scale.size()="<<scales.size();
        return image;
    }
    else{
        if(image.channels() < 5){
            cv::Scalar cv_means;
            cv::Scalar cv_scales;
            switch(image.channels()){
                case 1:
                    cv_means = cv::Scalar(means[0]);
                    cv_scales = cv::Scalar(scales[0]);
                    break;
                case 3:
                    cv_means = cv::Scalar(means[0], means[1], means[2]);
                    cv_scales = cv::Scalar(scales[0], scales[1], scales[2]);
                    break;
                case 4:
                    cv_means = cv::Scalar(means[0], means[1], means[2], means[3]);
                    cv_scales = cv::Scalar(scales[0], scales[1], scales[2], scales[3]);
                    break;
                default: {
                    LG << "Image not normalized as default case is reached";
                    return image;
                }
            }
            image = image - cv_means;
            // image = image / cv_scales;
            cv::divide(image, cv_scales, image);
        }
        else{
            
            std::vector<cv::Mat> image_channels;
            cv::split(image, image_channels);
            for(int i=0; i < image_channels.size(); i++){
                image_channels[i] = image_channels[i] - (means[i]); 
                image_channels[i] = image_channels[i] * (1.0f/scales[i]); 
                // cv::divide(image_channels[i], cv::Scalar(scales[i]), image_channels[i]); 
            }
            LG << "normalized | image channels with means & scale channels="<<image_channels.size();
            cv::Mat norm_image;
            cv::merge(image_channels, norm_image);
            return norm_image;
        }
    }
    return image;
}

std::vector<vizpet::BoundingBox> merge_pet_box_inside_petbox(std::vector<vizpet::BoundingBox> boxes){
  std::vector<cv::Rect> picked_pet_boxes;
  std::vector<vizpet::BoundingBox> pet_boxes;
  std::vector<vizpet::BoundingBox> picked;
  vizpet::Size size;
  if(boxes.size()>0){
      size=boxes[0].image_size;
  }
  // Merge pet box with in the other pet box
  for(int i = 0; i<boxes.size();i++){
    if(boxes[i].tag=="pets"){
      pet_boxes.push_back(boxes[i]);
    }else {
      picked.push_back(boxes[i]);
    }
  }
  int is_merged=0;
  for(int i =0;i<pet_boxes.size();i++){
    if(is_merged){
      i =0;
    }
    is_merged =0;
    for(int j =0;j < pet_boxes.size();j++){
      if(pet_boxes[i].location.left < pet_boxes[j].location.left && pet_boxes[i].location.top < pet_boxes[j].location.top 
        && pet_boxes[i].location.right > pet_boxes[j].location.right && pet_boxes[i].location.bottom > pet_boxes[i].location.bottom){
          // Index i is bigger box
          pet_boxes.erase(pet_boxes.begin()+j);
          is_merged =1;
          break;
        }
    }
  }
  /*for(int i=0;i<pet_boxes.size();i++){
    int is_not_merged=1;
    for(int j=i+1;j<pet_boxes.size();j++){
      float iou=RectangleUtils::compute_iou(pet_boxes[i].location,pet_boxes[j].location);
        LOGV("VZ Debug: get_merge_pet_boxes_hook [%f]", iou);
        if(iou>0.80){
          LOGV("VZ Debug: get_merge_pet_boxes_hook pet_boxes[%d].score : [%f], pet_boxes[%d].score : [%f]", i, pet_boxes[i].score, j, pet_boxes[j].score);
          vizpet::Rectangle rect_;
          rect_.left = std::min(pet_boxes[i].location.left,pet_boxes[j].location.left);
          rect_.top = std::min(pet_boxes[i].location.top,pet_boxes[j].location.top);
          rect_.right = std::max(pet_boxes[i].location.right,pet_boxes[j].location.right);
          rect_.bottom = std::max(pet_boxes[i].location.bottom,pet_boxes[j].location.bottom);
          /*std::vector<cv::Rect> temp_pet_boxes;
          temp_pet_boxes.push_back(cv::Rect(pet_boxes[i].location.left,pet_boxes[i].location.top,pet_boxes[i].location.width(),pet_boxes[i].location.height()));
          temp_pet_boxes.push_back(cv::Rect(pet_boxes[j].location.left,pet_boxes[j].location.top,pet_boxes[j].location.width(),pet_boxes[j].location.height()));
          cv::groupRectangles(temp_pet_boxes,1,99);
          if(temp_pet_boxes.size()>0){
            //picked_pet_boxes.push_back(temp_pet_boxes[0]);
            vizpet::Rectangle rect_=vizpet::Rectangle(temp_pet_boxes[0].x,
                                                    temp_pet_boxes[0].y,
                                                    temp_pet_boxes[0].x+temp_pet_boxes[0].width,
                                                    temp_pet_boxes[0].y+temp_pet_boxes[0].height);*/
            /*float score = (pet_boxes[i].score > pet_boxes[j].score) ? pet_boxes[i].score : pet_boxes[j].score;
            //LOGV("VZ Debug: get_merge_pet_boxes_hook i pet[%d].left : [%d], pet[%d].top : [%d], pet[%d].right : [%d], pet[%d].bottom : [%d]", i, pet_boxes[i].location.left, i, pet_boxes[i].location.top, i, pet_boxes[i].location.right, i, pet_boxes[i].location.bottom);
            //LOGV("VZ Debug: get_merge_pet_boxes_hook j pet[%d].left : [%d], pet[%d].top : [%d], pet[%d].right : [%d], pet[%d].bottom : [%d]", j, pet_boxes[j].location.left, j, pet_boxes[j].location.top, j, pet_boxes[j].location.right, j, pet_boxes[j].location.bottom);
            //LOGV("VZ Debug: get_merge_pet_boxes_hook temp_pet_boxes temp_pet_boxes[0].x : [%d], temp_pet_boxes[0].y : [%d], temp_pet_boxes[0].width : [%d], temp_pet_boxes[0].height : [%d]", temp_pet_boxes[0].x, temp_pet_boxes[0].y, temp_pet_boxes[0].width, temp_pet_boxes[0].height);
            //LOGV("VZ Debug: get_merge_pet_boxes_hook rect_ rect_.left : [%d], rect_.top : [%d], rect_.right : [%d], rect_.bottom : [%d]", rect_.left, rect_.top, rect_.right, rect_.bottom);
            //LOGV("VZ Debug: get_merge_pet_boxes_hook pet_boxes[%d].score : [%f], pet_boxes[%d].score : [%f], final_score : [%f]", i, pet_boxes[i].score, j, pet_boxes[j].score, score);
            picked.push_back(vizpet::BoundingBox(rect_,size,"pets",score,11));
            is_not_merged=0;
            pet_boxes.erase(pet_boxes.begin()+i);
            pet_boxes.erase(pet_boxes.begin()+j-1);
            i--;
            j-=2;
            break;
          //}
       // }
      //}
      /*if(is_not_merged)
        picked.push_back(pet_boxes[i]);
    }*/
    /*for(int i=0;i<picked_pet_boxes.size();i++){
        vizpet::Rectangle rect_=vizpet::Rectangle(picked_pet_boxes[i].x,
                                                        picked_pet_boxes[i].y,
                                                        picked_pet_boxes[i].x+picked_pet_boxes[i].width,
                                                        picked_pet_boxes[i].y+picked_pet_boxes[i].height);
        picked.push_back(vizpet::BoundingBox(rect_,size,"pets",0.98,11));
    }*/
    for(int i =0;i<pet_boxes.size();i++){
        picked.push_back(pet_boxes[i]);
    }
    #ifdef DUMPBOX
        LOGV("VZ Debug: merge_pet_boxes picked size : [%d]", picked.size());
    #endif
    return picked;
}

cv::Mat stabilize_for_illumination(const cv::Mat& input_image) {
    // Convert the input BGR image to LAB color space
    cv::Mat lab_image;
    cv::cvtColor(input_image, lab_image, cv::COLOR_BGR2Lab);

    // Split the LAB image into L, A, and B channels
    std::vector<cv::Mat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Apply Histogram Equalization to the L channel to stabilize illumination
    cv::equalizeHist(lab_channels[0], lab_channels[0]);

    // Merge the updated L channel with the A and B channels
    cv::merge(lab_channels, lab_image);

    // Convert the LAB image back to BGR color space
    cv::Mat stabilized_image;
    cv::cvtColor(lab_image, stabilized_image, cv::COLOR_Lab2BGR);

    // Enhance focus using Laplacian of Gaussian (LoG)
    /*cv::Mat gray_image;
    cv::cvtColor(stabilized_image, gray_image, cv::COLOR_BGR2GRAY);
    cv::Mat log_image;
    cv::GaussianBlur(gray_image, log_image, cv::Size(0, 0), 3);
    cv::addWeighted(gray_image, 1.5, log_image, -0.5, 0, log_image);
    cv::cvtColor(log_image, stabilized_image, cv::COLOR_GRAY2BGR);*/

    return stabilized_image;
}

cv::Mat stabilize_for_focus(const cv::Mat& input_image, double sigma = 1.0, double amount = 1.0) {
    // Split the input BGR image into separate channels
    std::vector<cv::Mat> bgr_channels;
    cv::split(input_image, bgr_channels);

    // Apply Unsharp Masking to each channel separately
    for (int i = 0; i < 3; ++i) {
        cv::Mat blurred_channel, high_freq, sharpened_channel;
        cv::GaussianBlur(bgr_channels[i], blurred_channel, cv::Size(0, 0), sigma);
        high_freq = bgr_channels[i] - blurred_channel;
        cv::addWeighted(bgr_channels[i], 1.0 + amount, high_freq, -amount, 0, sharpened_channel);
        bgr_channels[i] = sharpened_channel;
    }

    // Merge the channels back to form the stabilized BGR image
    cv::Mat stabilized_image;
    cv::merge(bgr_channels, stabilized_image);

    // Clip the pixel values to the valid range (0 to 255)
    cv::normalize(stabilized_image, stabilized_image, 0, 255, cv::NORM_MINMAX);
    stabilized_image.convertTo(stabilized_image, CV_8U);

    return stabilized_image;
}