#ifndef __DETECTOR_DECORATOR__
#define __DETECTOR_DECORATOR__

#include "../detector.hpp"
#include <opencv2/opencv.hpp>
#include <memory>

class DetectorDecorator: public IDetector{
public:
    DetectorDecorator(std::shared_ptr<IDetector> detector);
    ~DetectorDecorator();
    std::vector<vizpet::BoundingBox> detect(cv::Mat image);
    void set_tensor_parser(ROITensorParser tensor_parser);    

private:
    std::shared_ptr<IDetector> detector;
};

#endif //__DETECTOR_DECORATOR__