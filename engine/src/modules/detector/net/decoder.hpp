#ifndef DECODER_GHOSTDET_H
#define Decoder_H

#include "framework/tensor.hpp"
#include "model_config.hpp"
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

struct CenterPrior {
  int x;
  int y;
  int stride;
};

typedef struct BoxInfo {
  float x1;
  float y1;
  float x2;
  float y2;
  float score;
  int label;
} BoxInfo;

class Decoder {
public:
  Decoder(int _target_width,int _target_height, model::Version version);

  ~Decoder();

  // static Decoder* decoder;
  // modify these parameters to the same with your config if you want to use
  // your own model
  int target_width, target_height;
  int input_size[2] = {target_width, target_height}; // input height and width
  int num_class;             // number of classes. 80 for COCO
  int reg_max = 7; // `reg_max` set in the training config. Default: 7.
  const float score_threshold = 0.3;
	const float nms_threshold = 0.3;
  // std::vector<std::vector<float> > final_rois; // 4 coordinates 1 class and other confidence score.
  // std::vector<std::vector<BoxInfo>> results;
  // std::vector<float> box_vector;
  std::vector<int> strides_V1 = {8, 16, 32, 64}; // strides of the multi-level feature.
  std::vector<int> strides_V2 = {4, 8, 16, 32, 64};
  std::vector<std::vector<float> > decode(Tensor2D<float> &feats, float score_threshold,
                              float nms_threshold);
  void set_target_dimen(int _target_width,int _target_height);
  std::vector<std::string> v1_labels{"Background", "Objects", "Person", "PetFace", "HumanHead", "pets"};
  std::vector<std::string> v2_labels{"Background", "Objects", "Person", "PetFace", "HumanHead", "pets", "PetEyes"};
  std::vector<std::string> labels;
  std::vector<int> strides;

private:
  void decode_infer(Tensor2D<float> &feats,
                    std::vector<CenterPrior> &center_priors, float threshold,
                    std::vector<std::vector<BoxInfo>> &results);
  BoxInfo disPred2Bbox(const float *dfl_det, int label, float score, int x,
                       int y, int stride);
  void nms(std::vector<BoxInfo> &result, float nms_threshold);
  std::vector<CenterPrior> center_priors;

};

#endif // GhostDet_H
