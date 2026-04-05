//
// Reference RangiLyu from NCNN Repo (Nanodet)
// 2020 / 10 / 2

// Modified to ghostdet decoder by Biplab
// 2022 / 17 / 08
//

#include "decoder.hpp"
#include "logger/logger.hpp"
#include <iostream>
#include <cmath>

inline float fast_exp(float x) {
  union {
    uint32_t i;
    float f;
  } v{};
  v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
  return v.f;
}

inline float sigmoid(float x) { return 1.0f / (1.0f + fast_exp(-x)); }

template <typename _Tp>
int activation_function_softmax(const _Tp *src, _Tp *dst, int length) {
  const _Tp alpha = *std::max_element(src, src + length);
  _Tp denominator{0};

  for (int i = 0; i < length; ++i) {
    dst[i] = fast_exp(src[i] - alpha);
    //dst[i] = std::exp(src[i] - alpha);
    denominator += dst[i];
  }

  for (int i = 0; i < length; ++i) {
    dst[i] /= (denominator + 1e-8);
  }

  return 0;
}

static void
generate_grid_center_priors(const int input_height, const int input_width,
                            std::vector<int> &strides,
                            std::vector<CenterPrior> &center_priors) {
  for (int i = 0; i < (int)strides.size(); i++) {
    int stride = strides[i];
    int feat_w = ceil((float)input_width / stride);
    int feat_h = ceil((float)input_height / stride);
    for (int y = 0; y < feat_h; y++) {
      for (int x = 0; x < feat_w; x++) {
        CenterPrior ct;
        ct.x = x;
        ct.y = y;
        ct.stride = stride;
        center_priors.push_back(ct);
      }
    }
  }
}

// Decoder* Decoder::detector = nullptr;

Decoder::Decoder(int _target_width,int _target_height, model::Version version) {
  #ifdef DUMPBOX
    LOGV("VZ Debug:  Decoder::Decoder ");
  #endif
  if(version == model::Version::V2 || version == model::Version::V2_MULTI_FRAME){
    this->num_class = v2_labels.size();
    this->labels = v2_labels;
    this->strides = strides_V2;
    LOGV("VZ Debug:  Decoder::Decoder version 2 num_class : [%d], strides : [%d]", this->num_class, this->strides.size());
  }else {
    this->num_class = v1_labels.size();
    this->labels = v1_labels;
    this->strides = strides_V1;
    LOGV("VZ Debug:  Decoder::Decoder version 1 num_class : [%d], strides : [%d]", this->num_class, this->strides.size());
  }
  this->target_width = _target_width;
  this->target_height = _target_height;
    LOGV("VZ Debug:  Decoder::Decoder this->input_size[0] : [%d], this->input_size[1] :[%d]", this->target_width, this->target_height);
  generate_grid_center_priors(this->target_width, this->target_height,
                              this->strides, center_priors);
}

Decoder::~Decoder() {}

std::vector<std::vector<float> > Decoder::decode(Tensor2D<float> &feats,
                                             float score_threshold,
                                             float nms_threshold) {
  
  // LOGV("VZ Debug:  Decoder::num_class=%d", this->num_class);
  std::vector<std::vector<BoxInfo>> results;

  results.clear();
  results.resize(this->num_class);
  // generate center priors in format of (x, y, stride)
  // std::cout << "num priors: " << center_priors.size() << std::endl;
  // LOGV("VZ Debug:  Decoder::num priors: =[%d]", center_priors.size());
  // LOGV("VZ Debug:  Decoder::score_threshold=[%f], nms_threshold=[%f], num_class=%d", score_threshold, nms_threshold, this->num_class);
  this->decode_infer(feats, center_priors, score_threshold, results);

  //std::vector<BoxInfo> dets;
  std::vector<std::vector<float> > final_rois; // 4 coordinates 1 class and other confidence score.

  final_rois.clear();
  // box_vector.clear();
  // int box_vector_ptr = 0;
  // LOGV("VZ Debug:  Decoder::By pass nms");
  // return final_rois;
  for (int i = 0; i < (int)results.size(); i++) {
    // LOGV("VZ Debug:  Decoder::decode pre-nms resultssize[%d]=[%d]", i, results[i].size());
    this->nms(results[i], nms_threshold);
    // LOGV("VZ Debug:  Decoder::decode post-nms resultssize[%d]=[%d]", i, results[i].size());
    for (auto box : results[i]) {
      // box_vector.clear();
      // vector<int> box_vector = {box.x1 / target_width, 
      //           box.y1 / target_height, 
      //           box.x2 / target_width, 
      //           box.y2 / target_height, 
      //           box.score, (float) box.label};
      // LOGV("VZ Debug:  Decoder::decode post-nms results.label[%d] = %d", i, box.label);
      // final_rois.emplace_back(6);
      // final_rois.reserve(final_rois.size()+1);
      // final_rois[final_rois.size()-1].reserve(6);
      //dets.push_back(box);
      std::vector<float> box_vector = {box.x1 / target_width, 
                box.y1 / target_height, 
                box.x2 / target_width, 
                box.y2 / target_height, 
                box.score, (float) box.label};
      // final_rois[final_rois.size()-1]
      // final_rois[final_rois.size()-1][0] = (box.x1 / target_width);
      // final_rois[final_rois.size()-1][1] = (box.y1 / target_height);
      // final_rois[final_rois.size()-1][2] = (box.x2 / target_width);
      // final_rois[final_rois.size()-1][3] = (box.y2 / target_height);
      // final_rois[final_rois.size()-1][4] = (box.score);
      // final_rois[final_rois.size()-1][5] = ((float) box.label);
      
      // LOGV("VZ Debug:  Decoder::decode final_rois  x1= %f y1=%f x2= %f y2=%f box.score=%f box.label=%f", final_rois[final_rois.size()-1][0],final_rois[final_rois.size()-1][1], final_rois[final_rois.size()-1][2], final_rois[final_rois.size()-1][3], final_rois[final_rois.size()-1][4], final_rois[final_rois.size()-1][5]);
      final_rois.push_back(box_vector);
    }
  }

  // double end = ncnn::get_current_time();
  // double time = end - start;
  // printf("Detect Time:%7.2f \n", time);

  //return dets;
  return final_rois;
}

void Decoder::set_target_dimen(int _target_width,int _target_height){
  this->target_width = _target_width;
  this->target_height =_target_height;
  #ifdef DUMPBOX
    LOGV("Decoder : set_target_dimen : target_width : [%d], target_height : [%d]", this->target_width, this->target_height);
  #endif
}

void Decoder::decode_infer(Tensor2D<float> &feats,
                                   std::vector<CenterPrior> &center_priors,
                                   float threshold,
                                   std::vector<std::vector<BoxInfo>> &results) {
  const int num_points = center_priors.size();
  //LOGV("VZ Debug:  Decoder::decode_infer num_points = [%d]", num_points);
  // printf("num_points:%d\n", num_points);

  // cv::Mat debug_heatmap = cv::Mat(feature_h, feature_w, CV_8UC3);
  for (int idx = 0; idx < num_points; idx++) {
    const int ct_x = center_priors[idx].x;
    const int ct_y = center_priors[idx].y;
    const int stride = center_priors[idx].stride;

    //LOGV("VZ Debug:  Decoder::decode_infer idx = [%d]", idx);

    const float *scores = feats.row(idx);

    //LOGV("VZ Debug:  Decoder::decode_infer after scores= [0x%x]", scores);
    float score = -10000;
    int cur_label = 0;
    for (int label = 0; label < this->num_class; label++) {
      //LOGV("VZ Debug:  Decoder::decode_infer in label loop label=[%d], scores[label]=[%f]", label, scores[label]);
      if (scores[label] > score) {
        score = scores[label];
      
        cur_label = label;
      }
    }
    //LOGV("VZ Debug:  Decoder::decode_infer score > threshold curr_label=[%d], score = [%f], threshold=[%f]", cur_label, score, threshold);
    score = sigmoid(score);
    //LOGV("VZ Debug: Decoder::decode_infer end label:=[%d] score:[%f]", cur_label, score);
    if(score > threshold  && score <= 1.0){
      // std::cout << "label:" << cur_label << " score:" << score << std::endl;
      const float *bbox_pred = scores + this->num_class;
      results[cur_label].push_back(
          this->disPred2Bbox(bbox_pred, cur_label, score, ct_x, ct_y, stride));
      // debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
      // cv::imshow("debug", debug_heatmap);
    }
  }
  //LOGV("VZ Debug: Decoder::decode_infer end resultssize=[%d]", results.size());
}

BoxInfo Decoder::disPred2Bbox(const float *dfl_det, int label,
                                      float score, int x, int y, int stride) {
  float ct_x = x * stride;
  float ct_y = y * stride;
  std::vector<float> dis_pred;
  dis_pred.resize(4);
  const int REG_MAX = 7;
  float dis_after_sm[REG_MAX + 1];
  for (int i = 0; i < 4; i++) {
    float dis = 0;
    //float *dis_after_sm = new float[this->reg_max + 1];
    activation_function_softmax(dfl_det + i * (this->reg_max + 1), dis_after_sm,
                                this->reg_max + 1);
    for (int j = 0; j < this->reg_max + 1; j++) {
      dis += j * dis_after_sm[j];
    }
    dis *= stride;
    // std::cout << "dis:" << dis << std::endl;
    dis_pred[i] = dis;
    //delete[] dis_after_sm;
  }
  float xmin = (std::max)(ct_x - dis_pred[0], .0f);
  float ymin = (std::max)(ct_y - dis_pred[1], .0f);
  float xmax = (std::min)(ct_x + dis_pred[2], (float)this->target_width);
  float ymax = (std::min)(ct_y + dis_pred[3], (float)this->target_height);

  // std::cout << xmin << "," << ymin << "," << xmax << "," << xmax << "," <<
  // std::endl;
  return BoxInfo{xmin, ymin, xmax, ymax, score, label};
}

void Decoder::nms(std::vector<BoxInfo> &input_boxes, float NMS_THRESH) {
  std::sort(input_boxes.begin(), input_boxes.end(),
            [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
  std::vector<float> vArea(input_boxes.size());
  for (int i = 0; i < int(input_boxes.size()); ++i) {
    vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1) *
               (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
  }
  for (int i = 0; i < int(input_boxes.size()); ++i) {
    for (int j = i + 1; j < int(input_boxes.size());) {
      float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
      float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
      float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
      float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
      float w = (std::max)(float(0), xx2 - xx1 + 1);
      float h = (std::max)(float(0), yy2 - yy1 + 1);
      float inter = w * h;
      float ovr = inter / (vArea[i] + vArea[j] - inter + 1e-8);
      if (ovr >= NMS_THRESH) {
        input_boxes.erase(input_boxes.begin() + j);
        vArea.erase(vArea.begin() + j);
      } else {
        j++;
      }
    }
  }
}
