/**
 * @file   tag.hpp
 * @brief  Defines the structure and properties of tag
 *
 * Defines the structure and properties of tag
 * Tag defines the display, baseline information and their scores
 */

#ifndef __CORE_TAG__
#define __CORE_TAG__

#include <string>

namespace vizpet{
class Tag{
public:
    std::string display;
    std::string baseline;
    float display_score;
    float baseline_score;
    float display_threshold;

    Tag(){
        this->display = "None";
        this->baseline = "none";
        this->display_score = 0.0f;
        this->baseline_score = 0.0f;
    }
    Tag(std::string display_tag, float display_score, std::string baseline_tag, float baseline_score){
        this->display = display_tag;
        this->baseline = baseline_tag;
        this->display_score = display_score;
        this->baseline_score = baseline_score;
    }
    Tag(const Tag& tag){
        set(tag);
    }
    void set(const Tag& tag){
        this->display = tag.display;
        this->baseline = tag.baseline;
        this->display_score = tag.display_score;
        this->baseline_score = tag.baseline_score;
    }
};
} // viz

#endif //__CORE_TAG__