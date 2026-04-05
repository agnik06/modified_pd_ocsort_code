/**
 * @file   detector_net.hpp
 * @brief  This file provides interface for getting the outputs based on layer name
 *
 * This file provides interface for getting the outputs based on layer name
 */

#ifndef LightObjectDetector_HPP
#define LightObjectDetector_HPP

#include "net/NNet.hpp"
#include "framework/bounding_box.hpp"
#include "framework/tensor.hpp"
#include "decoder.hpp"
#include "model_config.hpp"

class DetectorNet: public NNet{
public:
    DetectorNet(NNet::Builder& builder,int target_width,int target_height, model::Version version);
    ~DetectorNet();
    /**
     * @brief gets the output based on layer name
     *
     * @param[in] layerName
     *
     * @return tensor with layer details
     */
    std::vector<std::vector<float> > final_boxes;
    virtual Tensor3D<float> get_output(const char* layerName);
    virtual std::vector<std::vector<float> > get_output_custom_decode_with_nms(const char* layerName1, const char* layerName2);
    virtual std::vector<std::vector<float> > get_output_customlayer(const char* layerName);

    Decoder* m_decoder;
};

#endif /* LightObjectDetector_HPP */

