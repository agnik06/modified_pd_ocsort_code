/**
 * @file   entity.hpp
 * @brief  This file defines the entity in code module
 *
 * This class provides appropriate properties for entity building.
 */

#ifndef __ENTITY__
#define __ENTITY__

#include <string>
#include <vector>

#include "opencv2/opencv.hpp"
#include "framework/rectangle.hpp"
#include "framework/bounding_box.hpp"
#include "framework/segment.hpp"
#include "framework/tag.hpp"

class Entity {
public:
  Entity();
  ~Entity();
  /**
   * @brief @constructor Builds an entity with 3 params
   *
   * @param[in] display_tag contains the tag
   * @param[in] display_tag_score contains the tag score
   * @param[in] roi_location contains the position of the box
   *
   * @return none
   */
  Entity(std::string display_tag, float display_tag_score, vizpet::Rectangle roi_location);
  /**
   * @brief @constructor Builds an entity with 3 params
   *
   * @param[in] box contains the bounding box
   * @param[in] tag stores the tag or label
   * @param[in] segment maintains the segment details
   *
   * @return none
   */
  Entity(vizpet::BoundingBox box, vizpet::Tag tag, vizpet::Segment segment);
  /**
   * @brief @constructor copy constructor
   */
  Entity(const Entity& other);

  /**
   * @brief Sets the values for roi, tag & mask
   *
   * @param[in] roi defines the bounding box
   * @param[in] tag defines the tag or label
   * @param[in] mask maintains the mask details
   *
   * @return none
   */
	void set(vizpet::BoundingBox roi, vizpet::Tag tag, cv::Mat mask);
	vizpet::BoundingBox roi;
	vizpet::Tag tag;
  vizpet::Segment segment;
};
#endif //__ENTITY__