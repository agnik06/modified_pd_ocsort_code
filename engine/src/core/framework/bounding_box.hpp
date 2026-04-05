/**
 * @file   bounding_box.hpp
 * @brief  Defines the structure and properties of bounding box
 *
 * Defines the structure and properties of bounding box
 */

#ifndef __CORE_BOUNDING_BOX__
#define __CORE_BOUNDING_BOX__

#include <string>
#include "rectangle.hpp"
#include "size.hpp"
#include "logger/logger.hpp"

namespace vizpet{
class BoundingBox{
public:
    Rectangle location;
    Size image_size;
    std::string tag;
    int tag_id;
    float score;
    int trackid;
    BoundingBox(){
        this->location = {0,0,0,0};
        this->image_size = {0,0};
        this->tag = "none";
        this->score = 0.0f;
        this->tag_id = -1;
        this->trackid = -1;
    }
    BoundingBox(Rectangle box_location, Size image_size, std::string box_tag, float box_score, int tag_id, int track_id = -1){
        //LOGV("VZ Debug: execute_with_tracking result_boxes track_id : [%d]", track_id);
        this->location = box_location;
        this->image_size = image_size;
        this->tag = box_tag;
        this->score = box_score;
        this->tag_id = tag_id;
        this->trackid =  track_id;
        //LOGV("VZ Debug: execute_with_tracking result_boxes this->trackid : [%d]", this->trackid);
    }
    BoundingBox(const BoundingBox& bbox){
        set(bbox);
    }
    void set(const BoundingBox& bbox){
        this->location = bbox.location;
        this->image_size = bbox.image_size;
        this->tag = bbox.tag;
        this->score = bbox.score;
        this->tag_id = bbox.tag_id;
        this->trackid = bbox.trackid;
    }
};
} // viz


#endif //__CORE_BOUNDING_BOX__