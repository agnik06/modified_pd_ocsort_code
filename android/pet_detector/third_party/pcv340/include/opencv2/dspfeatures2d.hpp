#ifndef __OPENCV_DSPFEATURES2D_HPP__
#define __OPENCV_DSPFEATURES2D_HPP__

#include "cvconfig.h"
#include "opencv2/core/base.hpp"
#include "opencv2/features2d.hpp"

namespace cv { namespace adsp {

/** @overload */
CV_EXPORTS void FAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression=true );

/** @brief Detects corners using the FAST algorithm

  This method is only supported on the EXYNOS8895 Device.

  @startparams
  @par{ image , grayscale image where keypoints (corners) are detected. } 
  @exypar{ Only supports CV_8UC1. }
  @par{ keypoints , keypoints detected on the image. } 
  @par{ threshold , threshold on difference between intensity of the central pixel and pixels of a
  circle around this pixel.}
  @par{ nonmaxSuppression , if true\, non-maximum suppression is applied to detected corners
  (keypoints).}
  @par{ type , type one of the three neighborhoods as defined in the paper:
  FastFeatureDetector::TYPE_9_16\, FastFeatureDetector::TYPE_7_12\,
  FastFeatureDetector::TYPE_5_8

  Detects corners using the FAST algorithm by @cite Rosten06 .}

  @note In Python API, types are given as cv2.FAST_FEATURE_DETECTOR_TYPE_5_8,
  cv2.FAST_FEATURE_DETECTOR_TYPE_7_12 and cv2.FAST_FEATURE_DETECTOR_TYPE_9_16. For corner
  detection, use cv2.FAST.detect() method.
  @exypar{ Only supports TYPE_9_16. } 
  @endparams

  Please refer to equivalent cv function.
  @see cv::FAST
 */
CV_EXPORTS void FAST( InputArray image, CV_OUT std::vector<KeyPoint>& keypoints,
                      int threshold, bool nonmaxSuppression, int type );
/** @brief Wrapping class for feature detection using the FAST method. :
    @see cv::FastFeatureDetector
 */
class CV_EXPORTS_W FastFeatureDetector : public cv::Feature2D
{
public:
    enum
    {
        TYPE_5_8 = 0, TYPE_7_12 = 1, TYPE_9_16 = 2,
        THRESHOLD = 10000, NONMAX_SUPPRESSION=10001, FAST_N=10002,
    };

    CV_WRAP static Ptr<cv::FastFeatureDetector> create( int threshold=10,
                                                    bool nonmaxSuppression=true,
                                                    int type=FastFeatureDetector::TYPE_9_16 );

    CV_WRAP virtual void setThreshold(int threshold) = 0;
    CV_WRAP virtual int getThreshold() const = 0;

    CV_WRAP virtual void setNonmaxSuppression(bool f) = 0;
    CV_WRAP virtual bool getNonmaxSuppression() const = 0;

    CV_WRAP virtual void setType(int type) = 0;
    CV_WRAP virtual int getType() const = 0;
};



}}



#endif // __OPENCV_FEATURES2D_HPP__
