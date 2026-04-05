/**
 * @file   bounding_box_hooks.hpp
 * @brief  This file provides interfaces for hooks
 *
 * This interfaces for the hooks are defined based on the custon need of entity
 */

#ifndef __BOUNDING_BOX_HOOK__
#define __BOUNDING_BOX_HOOK__

#include "framework/segment.hpp"
#include "framework/bounding_box.hpp"
#include "framework/tag.hpp"
#include "entity.hpp"
#include "utils/rectangle_utils.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include <vector>
#include <functional>

/**
 * Used to generate boxes.
 */
typedef std::function<std::vector<vizpet::BoundingBox> (std::vector<vizpet::BoundingBox> boxes, std::vector<vizpet::Tag> tags)> BoundingBoxGeneratorHook;

/**
 * Generates boxes merged according to thresholds
 * should be used when boxes nedds to be merged in order to remove overlapped boxes
 */
BoundingBoxGeneratorHook get_merge_category_boxes_hook(std::vector<std::string> &category,std::vector<float> &thresholds);

/**
 * Removes moon boxes based on the moon overlap threshold
 */
BoundingBoxGeneratorHook get_remove_object_moon_boxes_hook(float moon_overlap_threshold);
BoundingBoxGeneratorHook get_merge_pet_boxes_hook();

/**
 * Removes the object boxes
 */
BoundingBoxGeneratorHook get_remove_object_boxes_hook();

#endif //__BOUNDING_BOXHOOK__