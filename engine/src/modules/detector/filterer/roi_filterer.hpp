#ifndef __ROI_FILTERER_INTERFACE__
#define __ROI_FILTERER_INTERFACE__

#include "framework/bounding_box.hpp"
#include "../mapper/data/roi_data_handler.hpp"
#include "model_config.hpp"
#include "logger/logger.hpp"
#include <opencv2/opencv.hpp>
#include <memory>

class IROIFilterer{
public:
    IROIFilterer(std::shared_ptr<ROIDataHandler> data_handler);
    ~IROIFilterer();
    virtual std::vector<vizpet::BoundingBox> filter(std::vector<vizpet::BoundingBox> boxes, cv::Mat image) = 0;
    void set_reference_frame(vizpet::Rectangle reference_frame);
    void set_max_objects(int n);
    void setModelVersion(model::Version version);

protected:
    std::shared_ptr<ROIDataHandler> roi_data_handler;
    std::shared_ptr<vizpet::Rectangle> reference_frame;
    int max_object_count = 1;
    model::Version modelVersion;
    
};

#endif //__ROI_FILTERER_INTERFACE__