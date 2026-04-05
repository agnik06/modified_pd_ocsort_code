/**
 * @file   entity_hooks.cpp
 * @brief  Entity hooks helps to generate hooks from boxes
 *
 * This contains provides implementation for defining the entity hooks.
 */

#include "entity_hooks.hpp"

#include "logger/logger.hpp"

EntityGeneratorHook get_segment_entities_with_top_box_hook(){
    EntityGeneratorHook hook = [](std::vector<vizpet::Segment> segments, std::vector<vizpet::BoundingBox> boxes, std::vector<vizpet::Tag> tags){
        std::vector<Entity> entities;
        if(segments.empty()){
            return entities;
        }
        vizpet::BoundingBox top_box;
        if(!boxes.empty()){
            top_box = boxes[0];
        }
        for(auto segment: segments){
            entities.push_back(Entity(top_box, {}, segment));
        }
        return entities;
    };
    return hook;
}