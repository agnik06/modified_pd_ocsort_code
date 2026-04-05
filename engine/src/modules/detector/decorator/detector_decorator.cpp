#include "detector_decorator.hpp"

DetectorDecorator::DetectorDecorator(std::shared_ptr<IDetector> detector)
    :detector(detector){
    
}
DetectorDecorator::~DetectorDecorator(){

}

std::vector<vizpet::BoundingBox> DetectorDecorator::detect(cv::Mat image){
    assert(detector != nullptr && "Expected initialized Base-Detector");
    return detector->detect(image);
}

void DetectorDecorator::set_tensor_parser(ROITensorParser tensor_parser){
    assert(detector != nullptr && "Expected initialized Base-Detector");
    return detector->set_tensor_parser(tensor_parser);
}