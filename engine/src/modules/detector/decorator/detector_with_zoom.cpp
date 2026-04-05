#include "detector_with_zoom.hpp"
#include "../filterer/roi_utils.hpp"

DetectorWithZoom::DetectorWithZoom(std::shared_ptr<IDetector> detector)
    :DetectorDecorator(detector),
     margin_scale(0.01){

}
DetectorWithZoom::~DetectorWithZoom(){

}

std::vector<vizpet::BoundingBox> DetectorWithZoom::detect(cv::Mat image){
    std::vector<vizpet::BoundingBox> image_rois = DetectorDecorator::detect(image);

    cv::Rect zoom_rect;
    zoom_rect.x  =  margin_scale * image.size().width;
    zoom_rect.y   =  margin_scale * image.size().height;
    zoom_rect.width = image.size().width - 2 * zoom_rect.x;
    zoom_rect.height= image.size().height - 2 * zoom_rect.y;

    cv::Mat cropped_image = image(zoom_rect).clone();
    std::vector<vizpet::BoundingBox> cropped_image_rois = DetectorDecorator::detect(cropped_image);

    for(vizpet::BoundingBox& e: cropped_image_rois){
        e.location.left     += zoom_rect.x;
        e.location.top      += zoom_rect.y;
        e.location.right    += zoom_rect.x;
        e.location.bottom   += zoom_rect.y;
    }

    cropped_image_rois = ROIUtils::remove_overlapping_boxes(cropped_image_rois, image_rois);

    image_rois.insert(image_rois.end(), cropped_image_rois.begin(), cropped_image_rois.end());
    return image_rois;    
}