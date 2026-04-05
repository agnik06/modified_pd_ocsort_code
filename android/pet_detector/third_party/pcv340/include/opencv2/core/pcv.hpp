#ifndef OPENCV_CORE_GPU_HPP
#define OPENCV_CORE_GPU_HPP

/*******************************************************************************
*
* Advanced Visual R&D Group, Samsung Electronics 2016
*
* Scope of work: OpenCV Performance Optimization using OpenCL acceleration.
*
* 2016.07.28. add cv::gpu::gemm by Sungju Huh (sungju.huh@samsung.com)
* 2016.07.28. add cv::gpu::minMaxLoc by Sungju Huh (sungju.huh@samsung.com)
* 2016.07.28. add cv::gpu::minMaxIdx by Sungju Huh (sungju.huh@samsung.com)
* 2016.07.28. add cv::gpu::copyMakeBorder by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::divide by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::multiply by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::addWeighted by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::convertScaleAbs by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::magnitude by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::phase by Sungju Huh (sungju.huh@samsung.com)
* 2016.08.23. add cv::gpu::transpose by Sungju Huh (sungju.huh@samsung.com)
*
*******************************************************************************/

enum {
        IMPL_SERIAL = 0,
        IMPL_SOMP   = 1,
        IMPL_OCL    = 2
};

namespace cv
{
    namespace gpu
    {
/** @brief Performs generalized matrix multiplication on GPU device.

The function performs generalized matrix multiplication similar to the
gemm functions in BLAS level 3. For example,
`gemm(src1, src2, alpha, src3, beta, dst, GEMM_1_T + GEMM_3_T)`
corresponds to
\f[\texttt{dst} =  \texttt{alpha} \cdot \texttt{src1} ^T  \cdot \texttt{src2} +  \texttt{beta} \cdot \texttt{src3} ^T\f]

In case of complex (two-channel) data, performed a complex matrix
multiplication.

The function can be replaced with a matrix expression. For example, the
above call can be replaced with:
@code{.cpp}
    dst = alpha*src1.t()*src2 + beta*src3.t();
@endcode
@param src1 first multiplied input matrix that could be real(CV_32FC1,
CV_64FC1) or complex(CV_32FC2, CV_64FC2); dimension of src1 should not be
larger than 2; recommend to be declared with UMat for performance reason.
@param src2 second multiplied input matrix of the same type as src1;
dimension of src1 should not be larger than 2; recommend to be declared with
UMat for performance reason.
@param alpha weight of the matrix product.
@param src3 third optional delta matrix added to the matrix product; it
should have the same type as src1 and src2; dimension of src1 should not
be larger than 2; recommend to be declared with UMat for performance reason.
@param beta weight of src3.
@param dst output matrix; it has the proper size and the same type as
input matrices; should be declared with UMat.
@param flags operation flags (cv::GemmFlags)

@sa gpu::transform
*/
        CV_EXPORTS_W void gemm(InputArray src1, InputArray src2, double alpha,
                               InputArray src3, double beta, OutputArray dst, int flags = 0);

/** @brief Finds the global minimum and maximum in an array on GPU device.

The functions minMaxLoc find the minimum and maximum element values and their positions. The
extremums are searched across the whole array or, if mask is not an empty array, in the specified
array region.

The functions do not work with multi-channel arrays. If you need to find minimum or maximum
elements across all the channels, use Mat::reshape first to reinterpret the array as
single-channel. Or you may extract the particular channel using either extractImageCOI , or
mixChannels , or split .
@param src input single-channel array; dimension of src should not be larger than 2;
should be declared with UMat.
@param minVal pointer to the returned minimum value; NULL is used if not required.
@param maxVal pointer to the returned maximum value; NULL is used if not required.
@param minLoc pointer to the returned minimum location (in 2D case); NULL is used if not required.
@param maxLoc pointer to the returned maximum location (in 2D case); NULL is used if not required.
@param mask optional mask used to select a sub-array.
*/
        CV_EXPORTS_W void minMaxLoc(InputArray src, CV_OUT double* minVal,
                                    CV_OUT double* maxVal = 0, CV_OUT Point* minLoc = 0,
                                    CV_OUT Point* maxLoc = 0, InputArray mask = noArray());

/** @brief Finds the global minimum and maximum in an array on GPU device.

The function minMaxIdx finds the minimum and maximum element values and their positions. The
extremums are searched across the whole array or, if mask is not an empty array, in the specified
array region. The function does not work with multi-channel arrays. If you need to find minimum or
maximum elements across all the channels, use Mat::reshape first to reinterpret the array as
single-channel. Or you may extract the particular channel using either extractImageCOI , or
mixChannels , or split . In case of a sparse matrix, the minimum is found among non-zero elements
only.
@note When minIdx is not NULL, it must have at least 2 elements (as well as maxIdx), even if src is
a single-row or single-column matrix. In OpenCV (following MATLAB) each array has at least 2
dimensions, i.e. single-column matrix is Mx1 matrix (and therefore minIdx/maxIdx will be
(i1,0)/(i2,0)) and single-row matrix is 1xN matrix (and therefore minIdx/maxIdx will be
(0,j1)/(0,j2)).
@param src input single-channel arrayy; dimension of src should not be larger than 2;
should be declared with UMat.
@param minVal pointer to the returned minimum value; NULL is used if not required.
@param maxVal pointer to the returned maximum value; NULL is used if not required.
@param minIdx pointer to the returned minimum location (in nD case); NULL is used if not required;
Otherwise, it must point to an array of src.dims elements, the coordinates of the minimum element
in each dimension are stored there sequentially.
@param maxIdx pointer to the returned maximum location (in nD case). NULL is used if not required.
@param mask specified array region; if required, size of mask should be same as src.
*/
        CV_EXPORTS void minMaxIdx(InputArray src, double* minVal, double* maxVal = 0,
                                  int* minIdx = 0, int* maxIdx = 0, InputArray mask = noArray());

/** @brief Forms a border around an image on GPU device.

The function copies the source image into the middle of the destination image. The areas to the
left, to the right, above and below the copied source image will be filled with extrapolated
pixels. This is not what filtering functions based on it do (they extrapolate pixels on-fly), but
what other more complex functions, including your own, may do to simplify image boundary handling.

The function supports the mode when src is already in the middle of dst . In this case, the
function does not copy src itself but simply constructs the border, for example:

@code{.cpp}
    // let border be the same in all directions
    int border=2;
    // constructs a larger image to fit both the image and the border
    UMat gray_buf(rgb.rows + border*2, rgb.cols + border*2, rgb.depth());
    // select the middle part of it w/o copying data
    UMat gray(gray_canvas, Rect(border, border, rgb.cols, rgb.rows));
    // convert image from RGB to grayscale
    gpu::cvtColor(rgb, gray, COLOR_RGB2GRAY);
    // form a border in-place
    gpu::copyMakeBorder(gray, gray_buf, border, border,
                   border, border, BORDER_REPLICATE);
    // now do some custom filtering ...
    ...
@endcode
@note When the source image is a part (ROI) of a bigger image, the function will try to use the
pixels outside of the ROI to form a border. To disable this feature and always do extrapolation, as
if src was not a ROI, use borderType | BORDER_ISOLATED.

@param src Source image; dimension of src should not be larger than 2; recommend to be declared with UMat for performance reason.
@param dst Destination image of the same type as src and the size Size(src.cols+left+right,
src.rows+top+bottom); should be declared with UMat.
@param top
@param bottom
@param left
@param right Parameter specifying how many pixels in each direction from the source image rectangle
to extrapolate. For example, top=1, bottom=1, left=1, right=1 mean that 1 pixel-wide border needs
to be built.
@param borderType Border type. See borderInterpolate for details.
@param value Border value if borderType==BORDER_CONSTANT .
*/
        CV_EXPORTS_W void copyMakeBorder(InputArray src, OutputArray dst,
                                         int top, int bottom, int left, int right,
                                         int borderType, const Scalar& value = Scalar() );

/** @brief Performs per-element division of two arrays or a scalar by an array on GPU device.

The functions divide divide one array by another:
\f[\texttt{dst(I) = saturate(src1(I)*scale/src2(I))}\f]
or a scalar by an array when there is no src1 :
\f[\texttt{dst(I) = saturate(scale/src2(I))}\f]

When src2(I) is zero, dst(I) will also be zero. Different channels of
multi-channel arrays are processed independently.

@note Saturation is not applied when the output array has the depth CV_32S. You may even get
result of an incorrect sign in the case of overflow.
@param src1 first input array; dimension of src1 should not be larger than 2; recommend to be declared with UMat for performance reason.
@param src2 second input array of the same size and type as src1; recommend to be declared with UMat for performance reason.
@param scale scalar factor.
@param dst output array of the same size and type as src2; should be declared with UMat.
@param dtype optional depth of the output array; if -1, dst will have depth src2.depth(), but in
case of an array-by-array division, you can only pass -1 when src1.depth()==src2.depth().

@sa  gpu::multiply
*/
        CV_EXPORTS_W void divide(InputArray src1, InputArray src2, OutputArray dst,
                                 double scale = 1, int dtype = -1);

        CV_EXPORTS_W void divide(double scale, InputArray src2,
                                 OutputArray dst, int dtype = -1);

/** @brief Calculates the per-element scaled product of two arrays on GPU device.

The function multiply calculates the per-element product of two arrays:

\f[\texttt{dst} (I)= \texttt{saturate} ( \texttt{scale} \cdot \texttt{src1} (I)  \cdot \texttt{src2} (I))\f]

There is also a @ref MatrixExpressions -friendly variant of the first function. See Mat::mul .

For a not-per-element matrix product, see gemm .

@note Saturation is not applied when the output array has the depth
CV_32S. You may even get result of an incorrect sign in the case of
overflow.
@param src1 first input array; dimension of src1 should not be larger than 2; recommend to be declared with UMat for performance reason.
@param src2 second input array of the same size and the same type as src1; recommend to be declared with UMat for performance reason.
@param dst output array of the same size and type as src1; should be declared with UMat.
@param scale optional scale factor.
@param dtype optional depth of the output array

@sa gpu::divide, gpu::addWeighted, UMat::convertTo
*/
        CV_EXPORTS_W void multiply(InputArray src1, InputArray src2,
                                   OutputArray dst, double scale = 1, int dtype = -1);

/** @brief Calculates the weighted sum of two arrays on GPU device.

The function addWeighted calculates the weighted sum of two arrays as follows:
\f[\texttt{dst} (I)= \texttt{saturate} ( \texttt{src1} (I)* \texttt{alpha} +  \texttt{src2} (I)* \texttt{beta} +  \texttt{gamma} )\f]
where I is a multi-dimensional index of array elements. In case of multi-channel arrays, each
channel is processed independently.
The function can be replaced with a matrix expression:
@code{.cpp}
    dst = src1*alpha + src2*beta + gamma;
@endcode
@note Saturation is not applied when the output array has the depth CV_32S. You may even get
result of an incorrect sign in the case of overflow.
@param src1 first input array; dimension of src1 should not be larger than 2; recommend to be declared with UMat for performance reason.
@param alpha weight of the first array elements.
@param src2 second input array of the same size and channel number as src1; recommend to be declared with UMat for performance reason.
@param beta weight of the second array elements.
@param gamma scalar added to each sum.
@param dst output array that has the same size and number of channels as the input arrays; should be declared with UMat.
@param dtype optional depth of the output array; when both input arrays have the same depth, dtype
can be set to -1, which will be equivalent to src1.depth().

@sa  UMat::convertTo
*/
        CV_EXPORTS_W void addWeighted(InputArray src1, double alpha, InputArray src2,
                                      double beta, double gamma, OutputArray dst, int dtype = -1);

/** @brief Scales, calculates absolute values, and converts the result to 8-bit on GPU device.

On each element of the input array, the function convertScaleAbs
performs three operations sequentially: scaling, taking an absolute
value, conversion to an unsigned 8-bit type:
\f[\texttt{dst} (I)= \texttt{saturate\_cast<uchar>} (| \texttt{src} (I)* \texttt{alpha} +  \texttt{beta} |)\f]
In case of multi-channel arrays, the function processes each channel
independently. When the output is not 8-bit, the operation can be
emulated by calling the UMat::convertTo method (or by using matrix
expressions) and then by calculating an absolute value of the result.

@param src input array; dimention of src should not be larger than 2; recommend to be declared with UMat for performance reason.
@param dst output array; should be declared with UMat.
@param alpha optional scale factor.
@param beta optional delta added to the scaled values.

@sa  UMat::convertTo
*/
        CV_EXPORTS_W void convertScaleAbs(InputArray src, OutputArray dst,
                                          double alpha = 1, double beta = 0);

/** @brief Calculates the magnitude of 2D vectors on GPU device.

The function magnitude calculates the magnitude of 2D vectors formed
from the corresponding elements of x and y arrays:
\f[\texttt{dst} (I) =  \sqrt{\texttt{x}(I)^2 + \texttt{y}(I)^2}\f]
@param x floating-point array of x-coordinates of the vectors; dimension of x should not be larger than 2; recommend to be declared with UMat for performance reason.
@param y floating-point array of y-coordinates of the vectors; it must
have the same size as x; recommend to be declared with UMat for performance reason.
@param magnitude output array of the same size and type as x; should be declared with UMat

@sa gpu::phase
*/
        CV_EXPORTS_W void magnitude(InputArray x, InputArray y, OutputArray magnitude);

/** @brief Calculates the rotation angle of 2D vectors on GPU device.

The function phase calculates the rotation angle of each 2D vector that
is formed from the corresponding elements of x and y :
\f[\texttt{angle} (I) =  \texttt{atan2} ( \texttt{y} (I), \texttt{x} (I))\f]

The angle estimation accuracy is about 0.3 degrees. When x(I)=y(I)=0 ,
the corresponding angle(I) is set to 0.
@param x input floating-point array of x-coordinates of 2D vectors; dimension of x should not be larger than 2; recommend to be declared with UMat for performance reason.
@param y input array of y-coordinates of 2D vectors; it must have the
same size and the same type as x; recommend to be declared with UMat for performance reason.
@param angle output array of vector angles; it has the same size and
same type as x; should be declared with UMat.
@param angleInDegrees when true, the function calculates the angle in
degrees, otherwise, they are measured in radians.
*/

        CV_EXPORTS_W void phase(InputArray x, InputArray y, OutputArray angle,
                                bool angleInDegrees = false);

/** @brief Transposes a matrix on GPU device.

The function transpose transposes the matrix src :
\f[\texttt{dst} (i,j) =  \texttt{src} (j,i)\f]
@note No complex conjugation is done in case of a complex matrix. It it
should be done separately if needed.
@param src input array; dimension of src should not be larger than 2; recommend to be declared with UMat for performance reason.
@param dst output array of the same type as src; should be declared with UMat.
*/
        CV_EXPORTS_W void transpose(InputArray src, OutputArray dst);
    }
}

/*******************************************************************************
*
* Advanced Visual R&D Group, Samsung Electronics 2016
*
* Scope of work: OpenCV Performance Optimization using OpenMP acceleration.
*
* 2016.04.25. add cv::somp::LUT by Jeon Cheol-Yong(cy77.jeon@samsung.com)
* 2016.04.25. add cv::somp::mixChannels by Jeon Cheol-Yong(cyj77.eon@samsung.com)
*
*******************************************************************************/
namespace cv
{
    namespace somp
    {

        /** @brief The function somp::LUT works with Samsung OpenMP and somp::LUT is equivalent to cv::LUT.
          @param src input array of 8-bit elements.
          @param lut look-up table of 256 elements; in case of multi-channel input array, the table should
          either have a single channel (in this case the same table is used for all channels) or the same
          number of channels as in the input array.
          @param dst output array of the same size and number of channels as src, and the same depth as lut.
          @sa  convertScaleAbs, Mat::convertTo
          */
        CV_EXPORTS_W void LUT(InputArray src, InputArray lut, OutputArray dst);

        /** @brief The function somp::mixChannels works with Samsung OpenMP and somp::mixChannels is equivalent to cv::mixChannels.
          @param src input array or vector of matricesl; all of the matrices must have the same size and the
          same depth.
          @param nsrcs number of matrices in src.
          @param dst output array or vector of matrices; all the matrices *must be allocated*; their size and
          depth must be the same as in src[0].
          @param ndsts number of matrices in dst.
          @param fromTo array of index pairs specifying which channels are copied and where; fromTo[k\*2] is
          a 0-based index of the input channel in src, fromTo[k\*2+1] is an index of the output channel in
          dst; the continuous channel numbering is used: the first input image channels are indexed from 0 to
          src[0].channels()-1, the second input image channels are indexed from src[0].channels() to
          src[0].channels() + src[1].channels()-1, and so on, the same scheme is used for the output image
          channels; as a special case, when fromTo[k\*2] is negative, the corresponding output channel is
          filled with zero .
          @param npairs number of index pairs in fromTo.
          @sa split, merge, somp::cvtColor
          */
        CV_EXPORTS void mixChannels(const Mat* src, size_t nsrcs, Mat* dst, size_t ndsts,
            const int* fromTo, size_t npairs);

        /** @overload
          @param src input array or vector of matricesl; all of the matrices must have the same size and the
          same depth.
          @param dst output array or vector of matrices; all the matrices *must be allocated*; their size and
          depth must be the same as in src[0].
          @param fromTo array of index pairs specifying which channels are copied and where; fromTo[k\*2] is
          a 0-based index of the input channel in src, fromTo[k\*2+1] is an index of the output channel in
          dst; the continuous channel numbering is used: the first input image channels are indexed from 0 to
          src[0].channels()-1, the second input image channels are indexed from src[0].channels() to
          src[0].channels() + src[1].channels()-1, and so on, the same scheme is used for the output image
          channels; as a special case, when fromTo[k\*2] is negative, the corresponding output channel is
          filled with zero .
          @param npairs number of index pairs in fromTo.
          */
        CV_EXPORTS void mixChannels(InputArrayOfArrays src, InputOutputArrayOfArrays dst,
            const int* fromTo, size_t npairs);

        /** @overload
          @param src input array or vector of matricesl; all of the matrices must have the same size and the
          same depth.
          @param dst output array or vector of matrices; all the matrices *must be allocated*; their size and
          depth must be the same as in src[0].
          @param fromTo array of index pairs specifying which channels are copied and where; fromTo[k\*2] is
          a 0-based index of the input channel in src, fromTo[k\*2+1] is an index of the output channel in
          dst; the continuous channel numbering is used: the first input image channels are indexed from 0 to
          src[0].channels()-1, the second input image channels are indexed from src[0].channels() to
          src[0].channels() + src[1].channels()-1, and so on, the same scheme is used for the output image
          channels; as a special case, when fromTo[k\*2] is negative, the corresponding output channel is
          filled with zero .
          */
        CV_EXPORTS_W void mixChannels(InputArrayOfArrays src, InputOutputArrayOfArrays dst,
            const std::vector<int>& fromTo);

        /** @brief Please refer to equivalent function.
          @sa cv::extractChannel */
        CV_EXPORTS_W void extractChannel(InputArray src, OutputArray dst, int coi);

        /** @brief Please refer to equivalent function.
          @sa cv::insertChannel */
        CV_EXPORTS_W void insertChannel(InputArray src, InputOutputArray dst, int coi);

        /** @brief The function somp::kmeans works with Samsung OpenMP and somp::kmeans is equivalent to cv::kmeans.
          @param data Data for clustering. An array of N-Dimensional points with float coordinates is needed.
          Examples of this array can be:
          -   Mat points(count, 2, CV_32F);
          -   Mat points(count, 1, CV_32FC2);
          -   Mat points(1, count, CV_32FC2);
          -   std::vector\<cv::Point2f\> points(sampleCount);

          @param K Number of clusters to split the set by.
          @param bestLabels Input/output integer array that stores the cluster indices for every sample.
          @param criteria The algorithm termination criteria, that is, the maximum number of iterations and/or
          the desired accuracy. The accuracy is specified as criteria.epsilon. As soon as each of the cluster
          centers moves by less than criteria.epsilon on some iteration, the algorithm stops.
          @param attempts Flag to specify the number of times the algorithm is executed using different
          initial labellings. The algorithm returns the labels that yield the best compactness (see the last
          function parameter).
          @param flags Flag that can take values of cv::KmeansFlags
          @param centers Output matrix of the cluster centers, one row per each cluster center.
          @return The function returns the compactness measure that is computed as
          \f[\sum _i  \| \texttt{samples} _i -  \texttt{centers} _{ \texttt{labels} _i} \| ^2\f]
          after every attempt. The best (minimum) value is chosen and the corresponding labels and the
          compactness value are returned by the function. Basically, you can use only the core of the
          function, set the number of attempts to 1, initialize labels each time using a custom algorithm,
          pass them with the ( flags = KMEANS_USE_INITIAL_LABELS ) flag, and then choose the best
          (most-compact) clustering.
          */
        CV_EXPORTS_W double kmeans( InputArray data, int K, InputOutputArray bestLabels,
            TermCriteria criteria, int attempts,
            int flags, OutputArray centers = noArray() );


        /** @brief The function somp::addWeighted works with Samsung OpenMP and somp::addWeighted is equivalent to cv::addWeighted.

          @param src1 first input array; dimension of src1 should not be larger than 2; recommend to be declared with UMat for performance reason.
          @param alpha weight of the first array elements.
          @param src2 second input array of the same size and channel number as src1; recommend to be declared with UMat for performance reason.
          @param beta weight of the second array elements.
          @param gamma scalar added to each sum.
          @param dst output array that has the same size and number of channels as the input arrays; should be declared with UMat.
          @param dtype optional depth of the output array; when both input arrays have the same depth, dtype
          can be set to -1, which will be equivalent to src1.depth().

          @sa  UMat::convertTo
          */
        CV_EXPORTS_W void addWeighted(InputArray src1, double alpha, InputArray src2,
            double beta, double gamma, OutputArray dst, int dtype = -1);

        /** @brief Scales, calculates absolute values, and converts the result to 8-bit.
          @param src input array.
          @param dst output array.
          @param alpha optional scale factor.
          @param beta optional delta added to the scaled values.
          @sa  Mat::convertTo, cv::abs(const Mat&)
          */
        CV_EXPORTS_W void convertScaleAbs(InputArray src, OutputArray dst,
            double alpha = 1, double beta = 0);

    } // end of namespace somp
}// end of namespace cv

namespace cv
{
    namespace adsp
    {

        /** @brief Available dsp mode.

          see cv::adsp::initDsp, and cv::adsp::setDsp.
          */
        enum ADSP_MODE{
            ADSP_OFF         = -1,/*!<Turn off Dsp */
            ADSP_MINSVS      =  0,/*!<Set Dsp clock as MINSVS*/
            ADSP_LOWSVS      =  1,/*!<Set Dsp clock as LOWSVS*/
            ADSP_SVS         =  2,/*!<Set Dsp clock as SVS*/
            ADSP_NOMINAL     =  3,/*!<Set Dsp clock as NOMINAL*/
            ADSP_TURBO       =  4 /*!<Set Dsp clock as TURBO*/

        };

        /** @brief inquire dsp is supported or not.
        */
        CV_EXPORTS_W bool haveDsp();

        /** @brief Set dsp mode.

          This method is only supported on the Qualcomm Device.\n
          This can set following mode:
          - ADSP_OFF
          - ADSP_MINSVS
          - ADSP_LOWSVS
          - ADSP_NOMINAL
          - ADSP_TURBO
          @sa cv::adsp::ADSP_MODE
          */
        CV_EXPORTS_W bool setDsp(int mode);

        /** @brief Init dsp.

          This method is only supported on the Qualcomm Device.\n
          Default mode is ADSP_NOMINAL.\n
          This can set following mode:
          - ADSP_OFF
          - ADSP_MINSVS
          - ADSP_LOWSVS
          - ADSP_NOMINAL
          - ADSP_TURBO

          @sa cv::adsp::ADSP_MODE
          */
        CV_EXPORTS_W bool initDsp(int mode = ADSP_NOMINAL);

        /** @brief Deinit dsp.

          This method is only supported on the Qualcomm Device.\n
          Internally, it will set dsp down to ADSP_MINSVS clk.\n
          And when the process dies, dsp will be turned off.
          */
        CV_EXPORTS_W bool deinitDsp();

        /** @brief Check dsp is enabled or not.

          This method is only supported on the Qualcomm Device.
          */
        CV_EXPORTS_W bool isEnabled();

        /** @brief Check function is offloaded to dsp or redirects to cv function.
        */
        CV_EXPORTS_W bool offloadInfo(bool print = false);

        /** @brief Calculates the per-element sum of two arrays or an array and a scalar.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ src1 , first input array or a scalar. }
          @hexpar{ 128 Byte alignment needed. }
          @par{ src2 , second input array or a scalar. }
          @hexpar{ 128 Byte alignment needed.\n
          In case of either src1 or src2 is scalar, not supported.\n
          src1 and src2 must have the same size. }
          @par{ dst , output array that has the same size and number of channels as the input array(s); the
          depth is defined by dtype or src1/src2. }
          @par{ mask , optional operation mask - 8-bit single channel array. }
          @hexpar{ Not supported. }
          @par{ dtype , optional depth of the output array. }
          @hexpar{Only supports CV_8U, CV_16U, CV_16S, and CV_32S.}
          @endparams

          Please refer to equivalent cv function.
          @see cv::add
          */
        CV_EXPORTS_W void add(InputArray src1, InputArray src2, OutputArray dst,
            InputArray mask = noArray(), int dtype = -1);

        /** @brief Calculates the per-element difference between two arrays or an array and a scalar.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ src1 , first input array or a scalar. }
          @hexpar{ 128 Byte alignment needed. }
          @par{ src2 , second input array or a scalar. }
          @hexpar{ 128 Byte alignment needed.\n
          In case of either src1 or src2 is scalar, not supported.\n
          src1 and src2 must have the same size. }
          @par{ dst , output array that has the same size and number of channels as the input array(s); the
          depth is defined by dtype or src1/src2. }
          @par{ mask , optional operation mask - 8-bit single channel array. }
          @hexpar{ Not supported. }
          @par{ dtype , optional depth of the output array. }
          @hexpar{Only supports CV_8U, CV_16U, CV_16S, and CV_32S.}
          @endparams

          Please refer to equivalent cv function.
          @see cv::subtract
          */
        CV_EXPORTS_W void subtract(InputArray src1, InputArray src2, OutputArray dst,
            InputArray mask = noArray(), int dtype = -1);

        /** @brief Calculates the per-element scaled product of two arrays.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ src1 , first input array. }
          @hexpar{ 128 Byte alignment needed. }
          @par{ src2 , second input array. }
          @hexpar{ 128 Byte alignment needed.\n
          src1 and src2 must have the same size. }
          @par{ dst , output array of the same size and type as src1. }
          @par{ scale , optional scale factor. }
          @par{ dtype , optional depth of the output array. }
          @hexpar{Only supports CV_8U.}
          @endparams

          Please refer to equivalent cv function.
          @see cv::multiply
          */
        CV_EXPORTS_W void multiply(InputArray src1, InputArray src2,
            OutputArray dst, double scale = 1, int dtype = -1);

        /** @brief Calculates the per-element division of two arrays or a scalr by an array.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ src1 , first input array. }
          @hexpar{ 128 Byte alignment needed. }
          @par{ src2 , second input array of the same size and type as src1. }
          @hexpar{ 128 Byte alignment needed.\n
          In case of either src1 or src2 is scalar, not supported.\n
          src1 and src2 must have the same size. }
          @par{ dst , output array of the same size and type as src1. }
          @par{ scale , scale factor. }
          @par{ dtype , optional depth of the output array. }
          @hexpar{Only supports CV_8U.}
          @endparams

          Please refer to equivalent cv function.
          @see cv::divide
          */
        CV_EXPORTS_W void divide(InputArray src1, InputArray src2, OutputArray dst,
            double scale = 1, int dtype = -1);

        /** @overload */
        CV_EXPORTS_W void divide(double scale, InputArray src2,
            OutputArray dst, int dtype = -1);

        /** @brief Calculates the sum of array elements.

          @startparams
          @par{ src , input array that must have from 1 to 4 channels. }
          @exypar{ The depth is only supported in case of CV_8UC1. }
          @hexpar{ Only supports following type:
          - CV_8U &nbsp;&nbsp;: All channels
          - CV_16U : All channels
          - CV_16S : All channels}
          @endparams

          Please refer to equivalent cv function.
          @see cv::sum
          @sa  cv::countNonZero, cv::mean, cv::meanStdDev, cv::norm, cv::minMaxLoc, cv::reduce
          */
        CV_EXPORTS_W Scalar sum(InputArray _src);

        /** @brief Divides a multi-channel array into several single-channel arrays.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ _m , input multi-channel array. }
          @hexpar{ 128 Byte alignment needed.\n
          Only supports following type:
          - CV_8U &nbsp;&nbsp;: C2
          - CV_8S &nbsp;&nbsp;: C2
          - CV_16U : C2
          - CV_16S : C2
          - CV_32S : C2}
          @par{ _mv , output vector of arrays; the arrays themselves are reallocated\, if needed. }
          @endparams

          Please refer to equivalent cv function.
          @see cv::split
          @sa cv::merge, cv::mixChannels, cv::cvtColor
          */
        CV_EXPORTS_W void split(InputArray _m, OutputArrayOfArrays _mv);

        /** @brief Transposes a matrix.

          This method is only supported on the Qualcomm Device.

          @startparams
          @par{ src , input array. }
          @hexpar{ 128 Byte alignment needed.\n
          Only supports CV_8UC1. }
          @par{ dst , output array of the same type as src. }
          @endparams

          Please refer to equivalent cv function.
          @see cv::transpose
          */
        CV_EXPORTS_W void transpose(InputArray src, OutputArray dst);

        /** @brief Scales, calculates absolute values, and converts the result to 8-bit.

          This method is NOT supported on any architecture for now.

          @startparams
          @par{src, input array.}
          @par{dst, output array.}
          @par{alpha, optional scale factor.}
          @par{beta, optional delta added to the scaled values.}
          @endparams

          Please refer to equivalent cv function.
          @see cv::convertScaleAbs
          */
        CV_EXPORTS_W void convertScaleAbs(InputArray src, OutputArray dst, double alpha = 1, double beta = 0);

        /** @brief Finds the global minimum and maximum in an array.

        This method is only supported on the EXYNOS8895 Device.

        @startparams
        @par{src, input single-channel array.}
        @exypar{ Only supports CV_8UC1 and CV_16UC1.}
        @par{minVal, pointer to the returned minimum value; NULL is used if not required.}
        @par{maxVal, pointer to the returned maximum value; NULL is used if not required.}
        @par{minLoc, pointer to the returned minimum location (in 2D case); NULL is used if not required.}
        @par{maxLoc, pointer to the returned maximum location (in 2D case); NULL is used if not required.}
        @par{mask, optional mask used to select a sub-array.}
        @endparams

        Please refer to equivalent cv function.
        @see cv::minMaxLoc
        @sa max, min, compare, inRange, extractImageCOI, mixChannels, split, Mat::reshape
        */
        CV_EXPORTS_W void minMaxLoc(InputArray src, CV_OUT double* minVal,
                CV_OUT double* maxVal = 0, CV_OUT Point* minLoc = 0,
                CV_OUT Point* maxLoc = 0, InputArray mask = noArray());

        //! will update doxygen
        CV_EXPORTS_W void addWeighted(InputArray src1, double alpha, InputArray src2, double beta,
                                    double gamma, OutputArray dst, int dtype = -1);

    } //end of namespace adsp
} //end of namespace cv

#endif
