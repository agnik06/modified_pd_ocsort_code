/**
 * @file   default_roi_mapper.hpp
 * @brief  This file defines the mapping of boxes with labels
 *
 * This provides the interfaces for assigning the boxes with labels
 */

#ifndef __SSD_ROI_MAPPER__
#define __SSD_ROI_MAPPER__

#include "roi_mapper.hpp"
#include "framework/rectangle.hpp"

class DefaultROIMapper: public IROIMapper{
public:
    DefaultROIMapper(std::shared_ptr<ROIDataHandler> data_handler);
    ~DefaultROIMapper();

    /**
     * @brief maps the ROI bounding boxes
     *
     * @param[in] boxes representing bounding boxes
     *
     * @return bounding boxes
     */
    std::vector<vizpet::BoundingBox> map(std::vector<vizpet::BoundingBox> boxes);

private:
    /**
     * @brief assigns the labels to the bounding boxes
     *
     * @param[in] boxes representing the bounding boxes
     *
     * @return bounding boxes with labels set
     */
    std::vector<vizpet::BoundingBox> assign_labels(std::vector<vizpet::BoundingBox> boxes);
};

#endif //__SSD_ROI_MAPPER__