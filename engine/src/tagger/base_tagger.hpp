/**
 * @file   base_tagger.hpp
 * @brief  This file provides implementation of BaseTagger
 *
 * This class provides for implemetation of BaseTagger.
 */

#ifndef __BASE_TAGGER__
#define __BASE_TAGGER__

#include "modules/detector/detector.hpp"
#include "modules/detector/filterer/roi_filterer.hpp"

#include "modules/detector/mapper/roi_mapper.hpp"

#include "modules/detector/tracker_wrapper/tracker_wrapper.hpp"

#include "hooks/bounding_box_hooks.hpp"
#include "hooks/entity_hooks.hpp"

#include "core/entity.hpp"
#include <string>
#include <opencv2/opencv.hpp>
#include <memory>
#include "logger/timer.hpp"

class BaseTagger{
public:
    BaseTagger();
    ~BaseTagger();
    std::vector<vizpet::BoundingBox> get_boxes(cv::Mat image);
    std::vector<BoundingBoxGeneratorHook> bounding_box_hooks;
    void get_target_dimen(int& t_height,int& t_width);

protected:
    /**
     * @brief Sets the detector instance in tagger
     *
     * @param[in] detector associated with tagger
     *
     * @return none
     */
    void set_detector(std::shared_ptr<IDetector> detector);
    /**
     * @brief Sets the roi mapper in tagger
     *
     * @param[in] roi_mapper to set
     *
     * @return none
     */
    void set_roi_mapper(std::shared_ptr<IROIMapper> roi_mapper);
    /**
     * @brief Sets the roi filterer in tagger
     *
     * @param[in] roi_filterer to set
     *
     * @return none
     */
    void set_roi_filterer(std::shared_ptr<IROIFilterer> roi_filterer);

private:
    std::shared_ptr<IDetector> detector;
    std::shared_ptr<IROIMapper> roi_mapper;
    std::shared_ptr<IROIFilterer> roi_filterer;
};

#endif //__BASE_TAGGER__
