#include "roi_utils.hpp"

#include <numeric>

std::vector<vizpet::BoundingBox> ROIUtils::remove_smaller_boxes(std::vector<vizpet::BoundingBox> boxes, int img_w, int img_h, float ratio_threshold, int num_boxes){
    std::vector<vizpet::BoundingBox> selected_boxes;
    float area_image = (float)(img_w * img_h);
    float changed_threshold = 0.0;
    for (int i = 0; i < boxes.size(); i++) {
        changed_threshold = ratio_threshold;
        int l = boxes[i].location.left;
        int r = boxes[i].location.right;
        int t = boxes[i].location.top;
        int b = boxes[i].location.bottom;
        float area_box = (float)((r - l + 1) * (b - t + 1));
        float ratio = area_box / area_image;
        float aspect_ratio = float(r - l + 1) / float(b - t + 1);
        if (aspect_ratio < 0.5 || aspect_ratio > 2.0) {
            if (num_boxes == 1) {
                changed_threshold = 0.05;
            } else {
                changed_threshold = 0.02;
            }
        }
        if (ratio > changed_threshold) {
            selected_boxes.push_back(boxes[i]);
        }
    }
    return selected_boxes;
}

std::vector<vizpet::BoundingBox> ROIUtils::remove_smaller_boxes_for_Specific_categories(std::vector<vizpet::BoundingBox> boxes, int img_w, int img_h, int num_boxes){
    std::vector<vizpet::BoundingBox> selected_boxes;

    float area_image = (float)(img_w * img_h);
    for (int i = 0; i < boxes.size(); i++) {
        int l = boxes[i].location.left;
        int r = boxes[i].location.right;
        int t = boxes[i].location.top;
        int b = boxes[i].location.bottom;
        float area_box = (float)((r - l + 1) * (b - t + 1));
        float ratio = float(area_box) / float(area_image);
        int areaThreshold = ratio*100;
        int minAreaBoxThreshold = 0.00*100;//0.08*100; //considering smaller boxes for cats and dogs with ration < 8%
        

        //LOGV("ROIUtils: ROIUtils remove_smaller_boxes_for_Specific_categories box Tag is: %s and areaThreshold is: %d",boxes[i].tag.c_str(),areaThreshold);
        //if(!((boxes[i].tag == "flower" || boxes[i].tag == "animal") && (areaThreshold < minAreaBoxThreshold))){
        //if(!(boxes[i].tag == "Cats/Dogs" && (areaThreshold < minAreaBoxThreshold))){
        if(boxes[i].tag == "pets") {
            if((areaThreshold < minAreaBoxThreshold)){
                #ifdef DUMPBOX
                    LOGV("VZ Debug: remove_smaller_boxes_for_Specific_categories l=[%d],t: [%d], r=[%d],b=[%d],area=[%f],areaimage=[%f], ratio=[%f],areathreeshold=[%d]",l,t,r,b,area_box,area_image,ratio,areaThreshold);
	      #endif
            } else{
                selected_boxes.push_back(boxes[i]);
            }
        } else {
             selected_boxes.push_back(boxes[i]);
        }
    }
    return selected_boxes;
}

float ROIUtils::get_overlap_between_boxes(vizpet::Rectangle box1, vizpet::Rectangle box2) {
    //TOD0: Modify code to match get_iou_between_boxes style
    float box1_x1 = (float)(box1.left);
    float box1_y1 = (float)(box1.top);
    float box1_x2 = (float)(box1.right);
    float box1_y2 = (float)(box1.bottom);

    float box2_x1 = (float)(box2.left);
    float box2_y1 = (float)(box2.top);
    float box2_x2 = (float)(box2.right);
    float box2_y2 = (float)(box2.bottom);

    float x5 = std::max(box1_x1, box2_x1);
    float y5 = std::max(box1_y1, box2_y1);
    float x6 = std::min(box1_x2, box2_x2);
    float y6 = std::min(box1_y2, box2_y2);
    if ((x6 >= x5) && (y6 >= y5)) {
        float den = (float)((box2_x2 - box2_x1 + 1) * (box2_y2 - box2_y1 + 1));
        float overlap = ((float)((x6 - x5 + 1) * (y6 - y5 + 1))) / den;
        return overlap;
    }
    return 0.0;
}

float ROIUtils::get_iou_between_boxes(vizpet::Rectangle box1, vizpet::Rectangle box2){
    int inter_left      = std::max(box1.left,   box2.left);
    int inter_top       = std::max(box1.top,    box2.top);
    int inter_right     = std::min(box1.right,  box2.right);
    int inter_bottom    = std::min(box1.bottom, box2.bottom);
    vizpet::Rectangle intersect_box (inter_left, inter_top, inter_right, inter_bottom);
    if((inter_right >= inter_left) && (inter_bottom >= inter_top)){
        int intersection_area = intersect_box.area();
        int union_area = box1.area() + box2.area() - intersect_box.area();
        float overlap = (float)intersection_area / union_area;
        return overlap;
    }
    return 0.0;
}

//To remove ROIs from secondary boxes that overlap with primary boxes
std::vector<vizpet::BoundingBox> ROIUtils::remove_overlapping_boxes(std::vector<vizpet::BoundingBox> secondary_boxes,
                                                                 const std::vector<vizpet::BoundingBox> primary_boxes,
                                                                 float iou_threshold){
    secondary_boxes.erase(
        std::remove_if(secondary_boxes.begin(), secondary_boxes.end(), 
        [primary_boxes, iou_threshold](const vizpet::BoundingBox& secondary_box){
            for(vizpet::BoundingBox primary_box: primary_boxes){
                float overlap = get_iou_between_boxes(primary_box.location, secondary_box.location);
                if(overlap > iou_threshold){
                    return true;
                }
            }
            return false;
        }), secondary_boxes.end()
    );
    return secondary_boxes;
}

int ROIUtils::check_overlap_box(vizpet::Rectangle r1,vizpet::Rectangle r2,float area_threshold){
    int intersection=RectangleUtils::compute_intersection(r1,r2);
    if(r1.area()<r2.area()){    
        float overlap_s=(float)intersection/(float)r1.area();
        if((overlap_s>area_threshold)){
            return 0;
        }
        else return -1;
    }
    else{
        float overlap_s=(float)intersection/(float)r2.area();
        if((overlap_s>area_threshold)){
            return 1;
        }
        else return -1;
    }
}


std::vector<vizpet::BoundingBox> ROIUtils::remove_other_boxes_in_category_box(std::vector<vizpet::BoundingBox> boxes,std::vector<std::string> &categories,std::vector<float> &thresholds){
    // LOGV("CD Debug: Boxes size:%d",)
    std::set<int> removed;
    for(int i=0;i<categories.size();i++){
        std::string category = categories[i];
        float threshold = thresholds[i];
       
        for(int i=0;i<boxes.size();i++){
            if(boxes[i].tag==category.c_str()){
                for(int j=i+1;j<boxes.size();j++){
                    if(boxes[j].tag!=category.c_str()){
                        continue;
                    }
                    switch(ROIUtils::check_overlap_box(boxes[i].location,boxes[j].location,threshold)){
                        case 0:
                         removed.insert(i);
                         break;
                        case 1:
                         removed.insert(j);
                         break;
                        default:
                         continue;
                    }

                }
            }
        }
    }
    int i=0;
    for(auto val:removed){
        boxes.erase(boxes.begin()+val-i);
        i++;
    }
    return boxes;
}



std::vector<vizpet::BoundingBox> ROIUtils::remove_bigger_boxes(std::vector<vizpet::BoundingBox> all_boxes, int img_w, int img_h){
    std::vector<vizpet::BoundingBox> selected_boxes;
    std::vector<vizpet::BoundingBox> boxes;
    vizpet::BoundingBox save_box;
    save_box.location = {-1,-1,-1,-1};
    bool is_saved = false;
    for (int i = 0; i < all_boxes.size(); i++) {
        if (all_boxes[i].tag == "person") {
            selected_boxes.push_back(all_boxes[i]);
        } else {
            float area_ratio = all_boxes[i].location.area() / (img_w * img_h);
            if (area_ratio < 0.8) {
                boxes.push_back(all_boxes[i]);
            } else {
                save_box = all_boxes[i];
                is_saved = true;
            }
        }
    }

    std::vector<int> marker(boxes.size(), 0);
    for (int i = 0; i < boxes.size(); i++) {
        int no_overlaps = 0;
        for (int j = 0; j < boxes.size(); j++) {
            if (j != i && marker[j] == 0) {
                float overlap = get_overlap_between_boxes(boxes[i].location, boxes[j].location);
                if (overlap >= 0.5) {
                    no_overlaps += 1;
                }
            }
        }
        if (no_overlaps < 1) {
            selected_boxes.push_back(boxes[i]);
        } else {
            marker[i] = 1;
        }
    }

    if (selected_boxes.size() == 0 && is_saved) {
        selected_boxes.push_back(save_box);
    }
    return selected_boxes;
}

cv::Mat ROIUtils::getGaussianMatrix_HQ(cv::Mat image_src, int width, int height, double var, int mode){
    // this function is made by HQ and its original name(applied until Dream) is
    // getGaussianMatrix_HQ

    double xc = width * 0.5 - 0.5;
    double yc = height * 0.5 - 0.5;

    int sum_gray = 0;
    float avg = 0.0;
    double variance = 0.0;
    int diff_x, diff_y;
    int factor = MAX(width, height);
    double zero = 0.00000001;
    double scale_x, scale_y;
    float max_value = 0.0f;
    float cur_value;

    int imgSize = width * height;

    cv::Mat greyScale(image_src.rows, image_src.cols, CV_8UC1);
    cv::cvtColor(image_src, greyScale, cv::COLOR_BGR2GRAY);  // CV_YUV2BGR_NV21 = 93,

    cv::Mat resizedImg(height, width, CV_8UC1);

    cv::resize(greyScale, resizedImg, cv::Size(width, height), 0, 0,
                cv::INTER_LINEAR);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sum_gray += resizedImg.at<unsigned char>(y, x);
        }
    }
    avg = sum_gray / (imgSize);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            variance += (double)(avg - (float)resizedImg.at<unsigned char>(y, x)) *
                        (avg - (float)resizedImg.at<unsigned char>(y, x));
        }
    }
    variance /= (double)imgSize;

    cv::Mat gas(height, width, CV_32F);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double tmp =
                -((x - xc) * (x - xc) + (y - yc) * (y - yc)) / (variance + zero);
            cur_value = (float)exp(tmp);
            gas.at<float>(y, x) = cur_value;
            if (cur_value > max_value) max_value = cur_value;
        }
    }
    float quantizer = 1 / (max_value + 0.00001f);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cur_value = quantizer * gas.at<float>(y, x);
            gas.at<float>(y, x) = cur_value;
        }
    }

    return gas;
}

std::vector<vizpet::BoundingBox> ROIUtils::get_top_boxes(std::vector<vizpet::BoundingBox>& boxes, int img_w, int img_h, int num_boxes){
    std::vector<vizpet::BoundingBox> candidate_boxes;
    std::vector<vizpet::BoundingBox> top_boxes;
    int taken = 0;
    if (boxes.size() <= num_boxes) {
        return boxes;
    }
    else {
        std::vector<float> box_scores;
        for (vizpet::BoundingBox box : boxes) {
            float center_x = img_w * 0.5;
            float center_y = img_h * 0.5;
            vizpet::Rectangle loc;
            loc = box.location;

            float loc_y = (loc.bottom + loc.top) * 0.5;
            float loc_x = (loc.right + loc.left) * 0.5;
            float center_dist = sqrt((center_y - loc_y) * (center_y - loc_y) +
                                    (center_x - loc_x) * (center_x - loc_x));

            float area_box = (float)(loc.bottom - loc.top + 1) * (loc.right - loc.left + 1);
            box_scores.push_back(center_dist + (1.0 / area_box));
            candidate_boxes.push_back(box);
        }
        candidate_boxes = sort_boxes(candidate_boxes, box_scores);
        for(int i=0; i<MIN(candidate_boxes.size(), num_boxes); i++){
            top_boxes.push_back(candidate_boxes[i]);
        } 
    }

    return top_boxes;
}

double ROIUtils::getVariance_HQ(cv::Mat image_src, int width, int height, double var, int mode) {
    double xc = width * 0.5 - 0.5;
    double yc = height * 0.5 - 0.5;

    int sum_gray = 0;
    float avg = 0.0;
    double variance = 0.0;

    int imgSize = width * height;

    cv::Mat resizedImg(height, width, CV_8UC3);
    cv::resize(image_src, resizedImg, cv::Size(width, height), 0, 0, cv::INTER_LINEAR);

    cv::Mat greyScale(height, width, CV_8UC1);
    cv::cvtColor(resizedImg, greyScale, cv::COLOR_BGR2GRAY);  // CV_YUV2BGR_NV21 = 93,

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            sum_gray += greyScale.at<unsigned char>(y, x);
        }
    }
    avg = sum_gray / (imgSize);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            variance += (double)(avg - (float)greyScale.at<unsigned char>(y, x)) *
                        (avg - (float)greyScale.at<unsigned char>(y, x));
        }
    }
    variance /= (double)imgSize;

    resizedImg.release();
    greyScale.release();

    return variance;
}

double ROIUtils::getReverseGaussianScore_HQ(double variance, int width, int height,
                                  int left, int top, int right, int bottom) {
    double xc = width * 0.5 - 0.5;
    double yc = height * 0.5 - 0.5;

    double zero = 0.00000001;

    double merged, weight_TL, weight_BL, weight_TR, weight_BR;

    double tmp = -((left - xc) * (left - xc) + (top - yc) * (top - yc)) / (variance + zero);
    weight_TL = exp(tmp);

    tmp = -((left - xc) * (left - xc) + (bottom - yc) * (bottom - yc)) / (variance + zero);
    weight_BL = exp(tmp);

    tmp = -((right - xc) * (right - xc) + (top - yc) * (top - yc)) / (variance + zero);
    weight_TR = exp(tmp);

    tmp = -((right - xc) * (right - xc) + (bottom - yc) * (bottom - yc)) / (variance + zero);
    weight_BR = exp(tmp);

    weight_TL = 1.0f - weight_TL;
    weight_BL = 1.0f - weight_BL;
    weight_TR = 1.0f - weight_TR;
    weight_BR = 1.0f - weight_BR;

    merged = weight_TL + weight_BL + weight_TR + weight_BR;
    return merged;
}

std::vector<vizpet::BoundingBox> ROIUtils::get_top_boxes_HQ( std::vector<vizpet::BoundingBox>& boxes, int img_w, int img_h, cv::Mat image_src, int num_boxes){
    std::vector<vizpet::BoundingBox> candidate_boxes;
    std::vector<vizpet::BoundingBox> top_boxes;

    std::vector<vizpet::BoundingBox> inside_boxes;
    std::vector<vizpet::BoundingBox> outside_boxes;
    vizpet::Rectangle loc;
    int taken = 0;
    if (boxes.size() <= 1) {
        return boxes;
    }
     else {
        int adjust_size = MAX(MAX(img_w, img_h) / 10, 48); 
        // 48 is given value by Bixby vision 2.0 framework(they will give 480px image)
        int divider = MAX(img_w, img_h) / adjust_size;
        int adjustedW = img_w / divider;
        int adjustedH = img_h / divider;

        double variance_HQ = getVariance_HQ(image_src, adjustedW, adjustedH, 1.0f, 1);

        std::vector<float> inside_box_scores;
        std::vector<float> outside_box_scores;
        for (vizpet::BoundingBox box : boxes) {
            float loc_y, loc_x, center_dist, area_box, box_score, area_ratio;
            float center_x = img_w * 0.5;
            float center_y = img_h * 0.5;

            loc = box.location;

            loc_y = (loc.bottom + loc.top) * 0.5;
            loc_x = (loc.right + loc.left) * 0.5;

            int adjustedL = MAX(loc.left * adjustedW / img_w, 0);
            int adjustedT = MAX(loc.top * adjustedH / img_h, 0);
            int adjustedR = MIN(loc.right * adjustedW / img_w, adjustedW - 1);
            int adjustedB = MIN(loc.bottom * adjustedH / img_h, adjustedH - 1);

            double mergedGaussianValue = getReverseGaussianScore_HQ(
                variance_HQ, adjustedW, adjustedH, adjustedL, adjustedT, adjustedR,
                adjustedB);

            if (center_x >= loc.left && center_x <= loc.right &&
                center_y >= loc.top && center_y <= loc.bottom) {

                center_dist = sqrt((center_y - loc_y) * (center_y - loc_y) +
                                    (center_x - loc_x) * (center_x - loc_x));

                area_box = (float)(loc.bottom - loc.top + 1) * (loc.right - loc.left + 1);

                box_score = 1.0f - box.score;
                inside_box_scores.push_back(box_score + center_dist / area_box);
                inside_boxes.push_back(box);
            } else {
                box_score = 1.0f - box.score;
                area_ratio = ((loc.bottom - loc.top + 1) * (loc.right - loc.left + 1)) /
                                (float)(img_w * img_h);
                area_ratio = 1.0f - area_ratio;
                outside_box_scores.push_back(mergedGaussianValue + area_ratio + box_score);                
                outside_boxes.push_back(box);
            }
        }
        inside_boxes = sort_boxes(inside_boxes, inside_box_scores);
        outside_boxes = sort_boxes(outside_boxes, outside_box_scores);

        for (vizpet::BoundingBox box : inside_boxes) {
            loc = box.location;
            candidate_boxes.push_back(box);
        }
        for (vizpet::BoundingBox box : outside_boxes) {
            loc = box.location;
            candidate_boxes.push_back(box);
        }
        for (vizpet::BoundingBox box : candidate_boxes) {
            taken++;
            loc = box.location;
            top_boxes.push_back(box);
            if (taken >= num_boxes) {
                break;
            }
        }
    }

    return top_boxes;
}


bool ROIUtils::isCentered(vizpet::Rectangle roi_location, int width, int height, float margin){
    float margin_sqrt = sqrt(margin);
    vizpet::Rectangle center_rect = { (int)(width * (1 - margin_sqrt) * 0.5),
                             (int)(height * (1 - margin_sqrt) * 0.5),
                              (int)(width * (1 + margin_sqrt) * 0.5),
                             (int)(height * (1 + margin_sqrt) * 0.5)};

    int x = roi_location.get_center_x();
    int y = roi_location.get_center_y();
    if (center_rect.contains(x, y)) {
        return true;
    }
    return false;
}

std::vector<vizpet::BoundingBox> ROIUtils::sort_boxes(std::vector<vizpet::BoundingBox> boxes, std::vector<float> scores){
    if(boxes.size() != scores.size()){
        //Assert score for each box is available
        return {};
    }
    std::vector<int> indices(boxes.size());
    int idx = 0;
    std::iota(indices.begin(), indices.end(), idx++);
    std::sort(indices.begin(), indices.end(), [scores](int i, int j){
        return scores[i] < scores[j];
    });

    std::vector<vizpet::BoundingBox> sorted_boxes;
    for(int index: indices){
        sorted_boxes.push_back(boxes[index]);
    }
    return sorted_boxes;
}


std::vector<vizpet::BoundingBox> ROIUtils::soft_non_max_suppression_fast(std::vector<vizpet::BoundingBox>& boxes,
                                                  int imgW, int imgH) {
  std::vector<vizpet::BoundingBox> picked;
  if (boxes.size() == 0) {
    return std::vector<vizpet::BoundingBox>();
  }

  std::vector<float> x1;
  std::vector<float> y1;
  std::vector<float> x2;
  std::vector<float> y2;
  std::vector<float> scores;

  for (int i = 0; i < boxes.size(); i++) {
    x1.push_back((float)(boxes[i].location).left);
    y1.push_back((float)(boxes[i].location).top);
    x2.push_back((float)(boxes[i].location).right);
    y2.push_back((float)(boxes[i].location).bottom);
    scores.push_back((float)boxes[i].score);
  }

  while (boxes.size() != 0) {
    // max score box
    int index = std::max_element(scores.begin(), scores.end()) - scores.begin();

    if (scores[index] < 0.01) {
      break;
    }

    for (int j = 0; j < boxes.size(); j++) {
      if (j != index) {
        float x5 = std::max(x1[j], x1[index]);
        float y5 = std::max(y1[j], y1[index]);
        float x6 = std::min(x2[j], x2[index]);
        float y6 = std::min(y2[j], y2[index]);

        if ((x6 >= x5) && (y6 >= y5)) {
          float overlap = ((float)((x6 - x5 + 1) * (y6 - y5 + 1))) /
                          ((float)((x2[j] - x1[j] + 1) * (y2[j] - y1[j] + 1)));
          scores[j] = scores[j] * std::exp(-1.0 * 10.0 * overlap * overlap);
        }
      }
    }

    scores.erase(scores.begin() + index);
    x1.erase(x1.begin() + index);
    y1.erase(y1.begin() + index);
    x2.erase(x2.begin() + index);
    y2.erase(y2.begin() + index);
    picked.push_back(boxes[index]);
    boxes.erase(boxes.begin() + index);
    /*
    for(int j = 0; j < boxes.size(); j++)
    {
            if(scores[j] < 0.01)
            {
                    scores.erase(scores.begin() + j);
                    x1.erase(x1.begin() + j);
                    y1.erase(y1.begin() + j);
                    x2.erase(x2.begin() + j);
                    y2.erase(y2.begin() + j);
                    boxes.erase(boxes.begin() + j);
            }
    }
    */
  }
  std::cout << "success" << std::endl;
  return picked;
}
