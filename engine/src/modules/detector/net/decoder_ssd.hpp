#ifndef DECODER_SSD_H
#define DECODER_SSD_H

#include <iostream>
#include <vector>
//#include "centered_prior_boxes.hpp"
#include "framework/tensor.hpp"

class DecodeBoxesSSD
{
    public:
        DecodeBoxesSSD();
        static std::vector<std::vector<float> > decode(Tensor2D<float> mbox_loc);
};

#endif