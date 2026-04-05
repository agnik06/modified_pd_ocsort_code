/**
 * @file   entity_hooks.hpp
 * @brief  Entity hooks helps to generate hooks from boxes
 *
 * This contains interface for defining the entity hooks.
 */

#ifndef __ENTITY_HOOK__
#define __ENTITY_HOOK__

#include "framework/segment.hpp"
#include "framework/bounding_box.hpp"
#include "framework/tag.hpp"
#include "entity.hpp"
#include "opencv2/opencv.hpp"
#include <vector>
#include <functional>

/**
 * Used to generate entity from segments, boxes & tags.
 */
typedef std::function<std::vector<Entity> (std::vector<vizpet::Segment> segments, std::vector<vizpet::BoundingBox> boxes, std::vector<vizpet::Tag> tags)> EntityGeneratorHook;

/**
 * Generates entity for each segment with top box
 * should be used when segment is generated using top box
 */
EntityGeneratorHook get_segment_entities_with_top_box_hook();
#endif //__ENTITY_HOOK__