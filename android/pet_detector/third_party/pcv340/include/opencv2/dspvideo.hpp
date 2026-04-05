#ifndef __OPENCV_DSPVIDEO_HPP__
#define __OPENCV_DSPVIDEO_HPP__

#include "cvconfig.h"
#include "opencv2/core/base.hpp"
#include "opencv2/video.hpp"

namespace cv { namespace adsp {

/** @brief  Calculates an optical flow for a sparse feature set using the iterative Lucas-Kanade method with
    pyramids.

    This method has following constraints:
    - This method is only supported on the Qualcomm Device. 
    - This method is using FastCV C implementation. So, it will be offloaded as CPU. 
    - Because this method goes into FastCV domain and get back to ParallelCV domain, it might have different
    result.\n
    But it gets faster than original CV function.

    @startparams

    @par{ prevImg, first 8-bit input image or pyramid constructed by buildOpticalFlowPyramid.}
    @par{ nextImg, second input image or pyramid of the same size and the same type as prevImg.}
    @par{ prevPts, vector of 2D points for which the flow needs to be found; point coordinates must be
    single-precision floating-point numbers. }
    @par{ nextPts, output vector of 2D points (with single-precision floating-point coordinates)
    containing the calculated new positions of input features in the second image; when
    OPTFLOW_USE_INITIAL_FLOW flag is passed\, the vector must have the same size as in the input. }
    @par{ status, output status vector (of unsigned chars); each element of the vector is set to 1 if
    the flow for the corresponding features has been found\, otherwise\, it is set to 0. }
    @par{ err, output vector of errors; each element of the vector is set to an error for the
    corresponding feature\, type of the error measure can be set in flags parameter; if the flow wasn't
    found then the error is not defined (use the status parameter to find such cases). }
    @par{ winSize, size of the search window at each pyramid level. }
    @par{ maxLevel, 0-based maximal pyramid level number; if set to 0\, pyramids are not used (single
    level)\, if set to 1\, two levels are used\, and so on; if pyramids are passed to input then
    algorithm will use as many levels as pyramids have but no more than maxLevel. }
    @par{ criteria, parameter\, which specifying the termination criteria of the iterative search algorithm
    (after the specified maximum number of iterations criteria.maxCount or when the search window
    moves by less than criteria.epsilon. }
    @par{ flags, operation flags:
     -   **OPTFLOW_USE_INITIAL_FLOW** uses initial estimations\, stored in nextPts; if the flag is
         not set\, then prevPts is copied to nextPts and is considered the initial estimate.
     -   **OPTFLOW_LK_GET_MIN_EIGENVALS** use minimum eigen values as an error measure (see
         minEigThreshold description); if the flag is not set\, then L1 distance between patches
         around the original and a moved point\, divided by number of pixels in a window\, is used as a
         error measure. }
    @par{ minEigThreshold, the algorithm calculates the minimum eigen value of a 2x2 normal matrix of
    optical flow equations (this matrix is called a spatial gradient matrix in @cite Bouguet00)\, divided
    by number of pixels in a window; if this value is less than minEigThreshold\, then a corresponding
    feature is filtered out and its flow is not processed\, so it allows to remove bad points and get a
    performance boost. }
    @endparams

    Please refer to equivalent cv function.
    @see cv::calcOpticalFlowPyrLK

 */
CV_EXPORTS_W void calcOpticalFlowPyrLK( InputArray prevImg, InputArray nextImg,
                                        InputArray prevPts, InputOutputArray nextPts,
                                        OutputArray status, OutputArray err,
                                        Size winSize = Size(21,21), int maxLevel = 3,
                                        TermCriteria criteria = TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                        int flags = 0, double minEigThreshold = 1e-4 );
/** @brief  Calculates an optical flow for a sparse feature set using the iterative Lucas-Kanade method with
    pyramids.

    Please refer to following function.\n
    @ref cv::adsp::calcOpticalFlowPyrLK
*/

CV_EXPORTS_W void calcOpticalFlowPyrLK_v2( InputArray prevImg, InputArray nextImg,
                                        InputArray prevPts, InputOutputArray nextPts,
                                        OutputArray status, OutputArray err,
                                        Size winSize = Size(21,21), int maxLevel = 3,
                                        TermCriteria criteria = TermCriteria(TermCriteria::COUNT+TermCriteria::EPS, 30, 0.01),
                                        int flags = 0, double minEigThreshold = 1e-4 );

//! will update description
CV_EXPORTS_W Mat estimateRigidTransform(InputArray src1, InputArray src2, bool fullAffine);

}}

#endif // __OPENCV_DSPVIDEO_HPP__
