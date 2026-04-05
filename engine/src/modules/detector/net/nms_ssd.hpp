#ifndef __NMS__
#define __NMS__

#include <iostream>
#include <vector>
#include "framework/tensor.hpp"

class NMS_SSD{
    public:
      static std::vector<std::vector<float> > nms(std::vector<std::vector<float> > rois,Tensor2D<float> mbox_conf, int num_classes=5);

};

#endif