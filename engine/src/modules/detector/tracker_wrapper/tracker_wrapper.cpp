#include "tracker_wrapper.hpp"

TrackerWrapper::TrackerWrapper(){


}
TrackerWrapper::~TrackerWrapper(){

}

void TrackerWrapper::set_max_objects(int n){
    this->max_object_count = n;
}


void TrackerWrapper::set_tracker(std::vector<vizpet::BoundingBox> boxes, cv::Mat image){
    // LOGV("CD Debug: TrackerWrapper_set");
    tracked_boxes.clear();
    // for(int i=0;i<boxes.size();i++){
        if(boxes.empty()){
            return ;
        }
        else{
            cv::Point2f top_left=cv::Point2f(boxes[0].location.left,boxes[0].location.top),
                        bottom_right=cv::Point2f(boxes[0].location.right,boxes[0].location.bottom);
            int status=false;
            //status = tracker.set_tracker(image,top_left,bottom_right);
            if(status){
                boxes[0].tag=boxes[0].tag+"track";
                #ifdef DUMPBOX
                    LOGV("CD Debug: tracked_box_string=%s",boxes[0].tag.c_str());
                #endif
                tracked_boxes.push_back(boxes[0]);
            }
            else{
                tracked_boxes.clear();
                return;
            }
        }
        frame_count=0;
        return ;
        
    // }
    
}


bool TrackerWrapper :: update(cv::Mat image,vizpet::Rectangle old_box,vizpet::Rectangle &new_box){
    
    std::vector<cv::Point2f> nbox;
    //nbox =tracker.get_tracked_boxes(image);
    if(nbox.empty()){
        return 0;
    }
    else{
        new_box=vizpet::Rectangle(nbox[0].x,nbox[0].y,nbox[1].x,nbox[1].y);
        return 1;
    }
}


std::vector<vizpet::BoundingBox> TrackerWrapper::get_tracked_boxes( cv::Mat image){
    if(tracked_boxes.empty()){
        return {};
    }
    vizpet::Rectangle old_box=tracked_boxes[0].location,new_box;
    #ifdef DUMPBOX
        LOGV("CD Debug: TrackerWrapper_updating old_box_dim=%d,%d,%d,%d",old_box.top+1,old_box.left+1,old_box.bottom,old_box.right);
    #endif
    bool box_present=update(image,old_box,new_box);
    if(box_present){
        #ifdef DUMPBOX
            LOGV("CD Debug: TrackerWrapper_box_from_TrackerWrapper");
        #endif
        tracked_boxes[0].location=new_box;
        
        return tracked_boxes;
    }else{
        tracked_boxes.clear();
        return tracked_boxes;
    }
}

