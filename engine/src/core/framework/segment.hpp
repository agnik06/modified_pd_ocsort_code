#ifndef __CORE_SEGMENT__
#define __CORE_SEGMENT__

#include "opencv2/opencv.hpp"
#include <string>

namespace vizpet
{
class Segment{
public:
    cv::Mat mask;
    std::string tag;
    int tag_id;
    Segment(){
        this->tag = "none";
    }
    Segment(cv::Mat mask, std::string tag, int tag_id){
        this->mask = mask;
        this->tag = tag;
        this->tag_id = tag_id;
    }
    Segment(const Segment& segment){
        set(segment);
    }
    void set(const Segment& segment){
        this->mask = segment.mask;
        this->tag = segment.tag;
        this->tag_id = segment.tag_id;
    }
};
} // viz

#endif //__CORE_SEGMENT__