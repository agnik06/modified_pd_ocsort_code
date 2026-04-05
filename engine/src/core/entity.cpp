/**
 * @file   entity.cpp
 * @brief  This file implements the entity
 *
 * This class implements the entity structure which maintains the tag and location info.
 */
#include "entity.hpp"

/**
 * @brief @constructor Builds an entity with 3 params
 *
 * @param[in] display_tag contains the tag
 * @param[in] display_tag_score contains the tag score
 * @param[in] roi_location contains the position of the box
 *
 * @return none
 */
Entity::Entity(std::string display_tag,float display_tag_score, vizpet::Rectangle roi_location){
  this->tag.display = display_tag;
  this->tag.display_score = display_tag_score;
  this->roi.location = roi_location;
}

/**
 * @brief @constructor Builds an entity with 3 params
 *
 * @param[in] box contains the bounding box
 * @param[in] tag stores the tag or label
 * @param[in] segment maintains the segment details
 *
 * @return none
 */
Entity::Entity(vizpet::BoundingBox box, vizpet::Tag tag, vizpet::Segment segment){
  this->roi = box;
  this->tag = tag;
  this->segment = segment;
}

Entity::Entity(const Entity& e) {
  this->roi.set(e.roi);
  this->tag.set(e.tag);
  this->segment.set(e.segment);
}

Entity::Entity(){
  this->roi.tag = "none";
  this->roi.location = {0, 0, 0, 0};
  this->roi.score = -1;

  this->tag.baseline = "none";
  this->tag.baseline_score = -1;
  this->tag.display = "none";
  this->tag.display_score = -1;
}

Entity::~Entity() {}

/**
 * @brief Sets the values for roi, tag & mask
 *
 * @param[in] roi defines the bounding box
 * @param[in] tag defines the tag or label
 * @param[in] mask maintains the mask details
 *
 * @return none
 */
void Entity::set(vizpet::BoundingBox roi, vizpet::Tag tag, cv::Mat mask){
  this->roi = roi;
  this->tag = tag;
  this->segment.mask = mask;
}
