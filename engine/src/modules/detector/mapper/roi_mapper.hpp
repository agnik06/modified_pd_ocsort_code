/**
 * @file   roi_mapper.hpp
 * @brief  This file defines the interface for map
 *
 * This file defines the interface for map
 */

#ifndef __ROI_MAPPER__
#define __ROI_MAPPER__

#include "framework/bounding_box.hpp"
#include "framework/tensor.hpp"
#include "data/roi_data_handler.hpp"
#include "model_config.hpp"
#include "logger/logger.hpp"

#include <vector>
#include "opencv2/opencv.hpp"

class IROIMapper{
public:
    IROIMapper(std::shared_ptr<ROIDataHandler> data_handler);
    ~IROIMapper();
    virtual std::vector<vizpet::BoundingBox> map(std::vector<vizpet::BoundingBox> boxes) = 0;
    void setModelVersion(model::Version version);

protected:
    std::shared_ptr<ROIDataHandler> roi_data_handler;
    model::Version modelVersion;
};

#endif //__ROI_MAPPER__