#ifndef __OPENCV_DSPIMGPROC_HPP__
#define __OPENCV_DSPIMGPROC_HPP__

#include "cvconfig.h"
#include "opencv2/core/base.hpp"
#include "opencv2/imgproc.hpp"


namespace cv { namespace adsp {

	/** @brief Converts an image from one color space to another.
	
	@startparams
	@par{src, input image }
	@exypar{ Only supports CV_8U. }
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8U. }
	@par{dst, output image of the same size and depth as src.}
	@par{code, color space conversion code (see cv::ColorConversionCodes).}
	@exypar{Only supports CV_YUV2RGB_NV21.}
	@hexpar{Only supports CV_BGR2GRAY and CV_RGB2GRAY.}
	@par{dstCn, number of channels in the destination image; if the parameter is 0\, the number of the
	channels is derived automatically from src and code.}
	@exypar{ Only supports 1 channel. }
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::cvtColor
	@sa @ref imgproc_color_conversions
	 */
	CV_EXPORTS_W void cvtColor(InputArray src, OutputArray dst, int code, int dstCn = 0);

	/** @brief Harris corner detector.
	
	This method is only supported on the Qualcomm Device.

	@startparams
	@par{src, Input single-channel 8-bit or floating-point image.}
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8U. } 
	@par{dst, Image to store the Harris detector responses. It has the type CV_32FC1 and the same
	size as src.}
	@par{blockSize, Neighborhood size (see the details on cv::cornerEigenValsAndVecs ).}
	@par{ksize, Aperture parameter for the Sobel operator.}
	@par{k, Harris detector free parameter. See the formula below.}
	@par{borderType, Pixel extrapolation method. See cv::BorderTypes.}
	@hexpar{ Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101. }
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::cornerHarris
	 */
	CV_EXPORTS_W void cornerHarris(InputArray src, OutputArray dst, int blockSize,
								 int ksize, double k,
								 int borderType = BORDER_DEFAULT);

	/** @brief Calculates the minimal eigenvalue of gradient matrices for corner detection.
	
	@startparams
	@par{src, Input single-channel 8-bit or floating-point image.}
	@exypar{The width has to be greater than or equal to 384.\n
	Only supports CV_8UC1.}
	@hexpar{128 Byte alignment needed.\n
	The width has to be greater than or equal to 384.\n
	Only supports CV_8UC1.}
	@par{dst, Image to store the minimal eigenvalues. It has the type CV_32FC1 and the same size as
	src .}
	@par{blockSize, Neighborhood size (see the details on cv::cornerEigenValsAndVecs ).}
	@exypar{Only supports blockSize as 3.}
	@hexpar{Only supports blockSize as 3.}
	@par{ksize, Aperture parameter for the Sobel operator.}
	@exypar{Only supports blockSize as 3.}
	@hexpar{Only supports blockSize as 3.}
	@par{borderType, Pixel extrapolation method. See cv::BorderTypes.}
	@exypar{Only supports BORDER_REPLICATE.}
	@hexpar{Only supports BORDER_CONSTANT and BORDER_REFLECT_101.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::cornerMinEigenVal
	 */
	CV_EXPORTS_W void cornerMinEigenVal(InputArray src, OutputArray dst, int blockSize,
                                    int ksize = 3, int borderType = BORDER_DEFAULT);

	/** @brief Calculates the first, second, third, or mixed image derivatives using an extended Sobel operator.
	
	@startparams
	@par{src, input image.}
	@exypar{Only supports CV_8UC1.}
	@hexpar{128 Byte alignment needed.\n
	Only supports CV_8UC1.}
	@par{dst, output image of the same size and the same number of channels as src.}
	@exypar{Only supports CV_16SC1.}
	@hexpar{Only supports CV_8UC1, CV_16SC1, and CV_32FC1.}
	@par{ddepth, output image depth\, see @ref filter_depths "combinations"; in the case of
	    8-bit input images it will result in truncated derivatives.}
	@exypar{Only supports CV_16S.}
	@hexpar{Only supports CV_8U, CV_16S, and CV_32F.}
	@par{dx, order of the derivative x.}
	@exypar{Only supports following case:
				- dx, dy -> (1, 0)
				- dx, dy -> (0, 1).}
	@hexpar{Only supports following case:
				- dx, dy -> (1, 0)
				- dx, dy -> (0, 1)
				- dx, dy -> (1, 1).}
	@par{dy, order of the derivative y.}
	@exypar{please refer above.}
	@hexpar{please refer above.}
	@par{ksize, size of the extended Sobel kernel}
	@exypar{Only supports ksize as 3 and 5.}
	@hexpar{Only supports ksize as 3.}
	@par{scale, optional scale factor for the computed derivative values; by default\, no scaling is
	applied (see cv::getDerivKernels for details).}
	@exypar{Only supports scale as 1.}
	@hexpar{Supports in any case, but will go through Neon implementation.}
	@par{delta, optional delta value that is added to the results prior to storing them in dst.}
	@par{borderType, pixel extrapolation method\, see cv::BorderTypes.}
	@exypar{Only supports BORDER_CONSTANT and BORDER_REPLICATE.}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::Sobel
	@sa  Scharr, Laplacian, sepFilter2D, filter2D, GaussianBlur, cartToPolar
	 */
	CV_EXPORTS_W void Sobel(InputArray src, OutputArray dst, int ddepth,
                           int dx, int dy, int ksize = 3,
                           double scale = 1, double delta = 0,
                           int borderType = BORDER_DEFAULT);

	/** @brief Determines strong corners on an image.
	
	@startparams
	@par{image, Input 8-bit or floating-point 32-bit\, single-channel image.}
	@exypar{The width has to be greater than or equal to 384.\n Only supports CV_8UC1.}
	@hexpar{128 Byte alignment needed.\n
	The width has to be greater than or equal to 384.\n
	Onluy supports CV_8UC1.}
	@par{corners, Output vector of detected corners.}
	@par{maxCorners, Maximum number of corners to return. If there are more corners than are found\,
	the strongest of them is returned.}
	@par{qualityLevel, Parameter characterizing the minimal accepted quality of image corners. The
	parameter value is multiplied by the best corner quality measure\, which is the minimal eigenvalue
	\(see cornerMinEigenVal \) or the Harris function response \(see cornerHarris \). The corners with the
	quality measure less than the product are rejected. For example\, if the best corner has the
	quality measure = 1500\, and the qualityLevel=0.01\, then all the corners with the quality measure
	less than 15 are rejected.}
	@par{minDistance, Minimum possible Euclidean distance between the returned corners.}
	@par{mask, Optional region of interest. If the image is not empty (it needs to have the type
	CV_8UC1 and the same size as image )\, it specifies the region in which the corners are detected.}
	@par{blockSize, Size of an average block for computing a derivative covariation matrix over each
	pixel neighborhood. See cornerEigenValsAndVecs .}
	@par{useHarrisDetector, Parameter indicating whether to use a Harris detector (see cornerHarris)
	or cornerMinEigenVal.}
	@exypar{Not supported.}
	@hexpar{Not supported.}
	@par{k, Free parameter of the Harris detector.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::goodFeaturesToTrack
	@sa  cornerMinEigenVal, cornerHarris, calcOpticalFlowPyrLK, estimateRigidTransform,
	 */
	CV_EXPORTS_W void goodFeaturesToTrack(InputArray image, OutputArray corners,
                                        int maxCorners, double qualityLevel, double minDistance,
                                        InputArray mask = noArray(), int blockSize = 3,
                                        bool useHarrisDetector = false, double k = 0.04);

	/** @brief Finds line segments in a binary image using the probabilistic Hough transform.

	This method has following constraints:
		- This method is only supported on the Qualcomm Device.
		- This method is using FastCV C implementation. So, it will be offloaded as CPU.
		- Because this method goes into FastCV domain and get back to ParallelCV domain, it might have different
		result.\n
		But it gets faster than original CV function.

	@startparams
	@par{image, 8-bit\, single-channel binary source image. The image may be modified by the function.}
	@par{lines, Output vector of lines. Each line is represented by a 4-element vector
	\f$(x_1\, y_1\, x_2\, y_2)\f$ \, where \f$(x_1\,y_1)\f$ and \f$(x_2\, y_2)\f$ are the ending points of each detected
	line segment.}
	@par{rho, Distance resolution of the accumulator in pixels.}
	@par{theta, Angle resolution of the accumulator in radians.}
	@par{threshold, Accumulator threshold parameter. Only those lines are returned that get enough
	votes ( \f$>\texttt{threshold}\f$ ).}
	@par{minLineLength, Minimum line length. Line segments shorter than that are rejected.}
	@par{maxLineGap, Maximum allowed gap between points on the same line to link them.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::HoughLinesP
	@sa LineSegmentDetector
	 */
	CV_EXPORTS_W void HoughLinesP(InputArray image, OutputArray lines,
                                double rho, double theta, int threshold,
                                double minLineLength = 0, double maxLineGap = 0);

	/** @brief Erodes an image by using a specific structuring element.
	
	@startparams
	@par{src, input image}
	@exypar{Only supports CV_8UC1.}
	@hexpar{128 Byte alignment needed.\n
	Only supports CV_8UC1, CV_16SC1, and CV_16UC1.}
	@par{dst, output image of the same size and type as src.}
	@par{kernel, structuring element used for erosion}
	@exypar{Only supports kernel as 3x3 and 5x5 with Rectangular shape.}
	@hexpar{Only supports kernel as 3x3 with Rectangular, Ellipse, and CrossLane shape.}
	@par{anchor, position of the anchor within the element; default value (-1\, -1) means that the
	anchor is at the element center.}
	@exypar{Only supports default value\, (-1\, -1).}
	@hexpar{Only supports default value\, (-1\, -1).}
	@par{iterations, number of times erosion is applied.}
	@exypar{Only supports 1 iteration.}
	@hexpar{Only supports 1 iteration.}
	@par{borderType, pixel extrapolation method\, see cv::BorderTypes}
	@exypar{Only supports BORDER_CONSTANT as zero value.}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@par{borderValue, border value in case of a constant border}
	@exypar{Only supports border value as zero.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::erode
	@sa  dilate, morphologyEx, getStructuringElement
	 */
	CV_EXPORTS_W void erode(InputArray src, OutputArray dst, InputArray kernel,
                            Point anchor = Point(-1,-1), int iterations = 1,
                            int borderType = BORDER_CONSTANT,
                            const Scalar& borderValue = morphologyDefaultBorderValue());

	/** @brief Dilates an image by using a specific structuring element.
	
	@startparams
	@par{src, input image}
	@exypar{Only supports CV_8UC1.}
	@hexpar{128 Byte alignment needed.\n
	Only supports CV_8UC1, CV_16SC1, and CV_16UC1.}
	@par{dst, output image of the same size and type as src\`.}
	@par{kernel, structuring element used for erosion.}
	@exypar{Only supports kernel as 3x3 and 5x5 with Rectangular shape.}
	@hexpar{Only supports kernel as 3x3 with Rectangular, Ellipse, and CrossLane shape.}
	@par{anchor, position of the anchor within the element; default value (-1\, -1) means that the
	anchor is at the element center.}
	@exypar{Only supports default value\, (-1\, -1).}
	@hexpar{Only supports default value\, (-1\, -1).}
	@par{iterations, number of times dilation is applied.}
	@exypar{Only supports 1 iteration.}
	@hexpar{Only supports 1 iteration.}
	@par{borderType, pixel extrapolation method\, see cv::BorderTypes.}
	@exypar{Only supports BORDER_CONSTANT as zero value.}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@par{borderValue, border value in case of a constant border}
	@exypar{Only supports border value as zero.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::dilate
	@sa  erode, morphologyEx, getStructuringElement
	 */
	CV_EXPORTS_W void dilate(InputArray src, OutputArray dst, InputArray kernel,
                            Point anchor = Point(-1,-1), int iterations = 1,
                            int borderType = BORDER_CONSTANT,
                            const Scalar& borderValue = morphologyDefaultBorderValue());

	/** @brief Performs advanced morphological transformations.
	
	This method is only supported on the Qualcomm Device.

	@startparams
	@par{src, Source image. The number of channels can be arbitrary. The depth should be one of
	CV_8U\, CV_16U\, CV_16S\, CV_32F or CV_64F.}
	@hexpar{128 Byte alignment needed.\n
	Only supports CV_8UC1, CV_16SC1, and CV_16UC1.}
	@par{dst, Destination image of the same size and type as  src\`.}
	@par{kernel, Structuring element. It can be created using getStructuringElement.}
	@hexpar{Only supports kernel as 3x3 with Rectangular, Ellipse, and CrossLane shape.}
	@par{op, Type of a morphological operation\, see cv::MorphTypes}
	@hexpar{Only supports MORPH_DILATE, MORPH_ERODE, MORPH_OPEN, and MORPH_CLOSE.}
	@par{anchor, Anchor position with the kernel. Negative values mean that the anchor is at the
	kernel center.}
	@hexpar{Only supports default value\, (-1\, -1).}
	@par{iterations, Number of times erosion and dilation are applied.}
	@hexpar{Only supports 1 iteration.}
	@par{borderType, Pixel extrapolation method\, see cv::BorderTypes}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@par{borderValue, Border value in case of a constant border. The default value has a special
	meaning.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::morphologyEx
	@sa  dilate, erode, getStructuringElement
	 */
	CV_EXPORTS_W void morphologyEx( InputArray src, OutputArray dst,
                                int op, InputArray kernel,
                                Point anchor = Point(-1,-1), int iterations = 1,
                                int borderType = BORDER_CONSTANT,
                                const Scalar& borderValue = morphologyDefaultBorderValue() );

	/** @brief Resizes an image.
	
	@startparams
	@par{src, input image.}
	@exypar{ Only supports CV_8UC1, CV_8UC3 and CV_8UC4.}
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8UC1, CV_8UC3 and CV_8UC4.\n
	The width has to be greater than or equal to 64.\n
	The height has to be greater than or equal to 4.}
	@par{dst, output image; it has the size dsize (when it is non-zero) or the size computed from
	src.size()\, fx\, and fy; the type of dst is the same as of src.}
	@par{dsize, output image size; if it equals zero\, it is computed as:
	 \f[\texttt{dsize = Size(round(fx*src.cols)\, round(fy*src.rows))}\f]
	 Either dsize or both fx and fy must be non-zero.}
	@par{fx, scale factor along the horizontal axis; when it equals 0\, it is computed as
	\f[\texttt{(double)dsize.width/src.cols}\f]}
	@exypar{ Only supports following cases: 0.25, 0.5, 2, 4.}
	@par{fy, scale factor along the vertical axis; when it equals 0\, it is computed as
	\f[\texttt{(double)dsize.height/src.rows}\f]}
	@exypar{ Only supports following cases: 0.25, 0.5, 2, 4.}
	@par{interpolation, interpolation method\, see cv::InterpolationFlags}
	@exypar{ Only supports INTER_NEAREST.}
	@hexpar{ Only supports INTER_NEAREST, INTER_BILINEAR, and INTER_BICUBIC.}
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::resize
	@sa  warpAffine, warpPerspective, remap
	 */
	CV_EXPORTS_W void resize(InputArray src, OutputArray dst,
							Size dsize, double inv_scale_x = 0, double inv_scale_y = 0,
			                int interpolation = INTER_LINEAR);
	
	/** @brief Blurs an image using the normalized box filter. 
          
	This method is only supported on the Qualcomm Device.
	
	@startparams
	@par{ src , input image; it can have any number of channels\, which are processed independently\, but
	the depth should be CV_8U\, CV_16U\, CV_16S\, CV_32F or CV_64F.} 
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8U and CV_16S with all kinds of channels. } 
	@par{ dst , output image of the same size and type as src. } 
	@hexpar{ Should be same with src type and size. } 
	@par{ ksize , blurring kernel size. }
	@hexpar{ In case of ksize.width * ksize.height has to be less than 127. } 
	@par{ anchor , anchor point; default value Point(-1\,-1) means that the anchor is at the kernel
	center. }
	@hexpar{Only supports default value\, (-1\, -1).}
	@par{ borderType , border mode used to extrapolate pixels outside of the image\, see cv::BorderTypes }
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@endparams 

	Please refer to equivalent cv function.
	@see cv::blur
	*/ 
	CV_EXPORTS_W void blur(InputArray src, OutputArray dst,
						Size ksize, Point anchor = Point(-1,-1),
						int borderType = BORDER_DEFAULT);

	/** @brief Blurs an image using the box filter.

	@startparams
	@par{ src , input image } 
	@exypar{ Only supports CV_8UC1. }
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8U and CV_16S with all kinds of channels. } 
	@par{ dst , output image of the same size and type as src. } 
	@exypar{ Should be same with src type and size. } 
	@hexpar{ Should be same with src type and size. } 
	@par{ ddepth , the output image depth }
	@exypar{ Only supports CV_8U. }
	@hexpar{ Only supports CV_8U and CV_16S. } 
	@par{ ksize , blurring kernel size. }
	@exypar{ Only supports as 3x3 and 5x5. }
	@hexpar{ In case of ksize.width * ksize.height has to be less than 127. } 
	@par{ anchor , anchor point; default value Point(-1\,-1) means that the anchor is at the kernel
	center. }
	@exypar{Only supports default value\, (-1\, -1).}
	@hexpar{Only supports default value\, (-1\, -1).}
	@par{ normalize , flag\, specifying whether the kernel is normalized by its area or not.}
	@exypar{ Only supports true case. }
	@par{ borderType , border mode used to extrapolate pixels outside of the image\, see cv::BorderTypes }
	@exypar{Only supports BORDER_CONSTANT and BORDER_REPLICATE.}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@endparams 

	Please refer to equivalent cv function.
	@see cv::boxFilter
	*/
	CV_EXPORTS_W void boxFilter(InputArray src, OutputArray dst, int ddepth,
						Size ksize, Point anchor = Point(-1,-1),
						bool normalize = true,
						int borderType = BORDER_DEFAULT);

	/** @brief Blurs an image using a Gaussian filter.

	@startparams
	@par{ src , input image } 
	@exypar{ Only supports CV_8UC1. }
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8UC1. } 
	@par{ dst , output image of the same size and type as src. } 
	@exypar{ Should be same with src type.}
	@hexpar{ Should be same with src type.}
	@par{ ksize , blurring kernel size. }
	@exypar{ Only supports ksize as 3x3 and 5x5. } 
	@hexpar{ Only supports ksize as 3x3, 5x5, and 7x7. } 
	@par{ sigmaX , Gaussian kernel standard deviation in X direction. }
	@par{ sigmaY , Gaussian kernel standard deviation in Y direction; if sigmaY is zero\, it is set to be
	equal to sigmaX\, if both sigmas are zeros\, they are computed from ksize.width and ksize.height\,
	respectively (see cv::getGaussianKernel for details); to fully control the result regardless of
	possible future modifications of all this semantics\, it is recommended to specify all of ksize\,
	sigmaX\, and sigmaY.} 
	@par{ borderType , borderType pixel extrapolation method\, see cv::BorderTypes }
	@exypar{Only supports BORDER_CONSTANT and BORDER_REPLICATE.}
	@hexpar{Only supports BORDER_CONSTANT, BORDER_REPLICATE, and BORDER_REFLECT_101.}
	@endparams 

	Please refer to equivalent cv function.
	@see cv::GaussianBlur
	*/
	CV_EXPORTS_W void GaussianBlur(InputArray src, OutputArray dst, Size ksize,
							double sigmaX, double sigmaY = 0,
							int borderType = BORDER_DEFAULT);

	/** @brief blurs an image using the median filter.

	@startparams
	@par{ src , input 1-\, 3-\, or 4-channel image; when ksize is 3 or 5\, the image depth should be
	CV_8U\, CV_16U\, or CV_32F\, for larger aperture sizes\, it can only be CV_8U.} 
	@exypar{ Only supports CV_8UC1. } 
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8UC1. } 
	@par{ dst , destination array of the same size and type as src } 
	@par{ ksize , ksize aperture linear size; it must be odd and greater than 1\, for example: 3\, 5\, 7 ... }
	@exypar{ Only supports ksize as 3x3 and 5x5. } 
	@hexpar{ Only supports ksize as 3x3. } 
	@endparams 

	Please refer to equivalent cv function.
	@see cv::medianBlur
	*/
	CV_EXPORTS_W void medianBlur(InputArray src, OutputArray dst, int ksize);

	/** @brief Applies a fixed-level threshold to each array element. 
          
	This method is only supported on the Qualcomm Device.
	
	@startparams
	@par{ src , input array (single-channel\, 8-bit or 32-bit floating point).} 
	@hexpar{ 128 Byte alignment needed.\n
	Only supports CV_8U and CV_16S with all kinds of channels. } 
	@par{ dst , output array of the same size and type as src. } 
	@par{ thresh , threshold value. } 
	@par{ maxval , maximum value to use with the THRESH_BINARY and THRESH_BINARY_INV thresholding
	types.} 
	@par{ type , thresholding type (see the cv::ThresholdTypes). }  
	@hexpar{ Supports THRESH_BINARY ,THRESH_BINARY_INV, THRESH_TRUNC, THRESH_TOZERO, and THRESH_TOZERO_INV. } 
	@endparams 

	Please refer to equivalent cv function.
	@see cv::threshold
	*/
	CV_EXPORTS_W double threshold(InputArray src, OutputArray dst,
                                double thresh, double maxval, int type);

	/** @brief Equalizes the histogram of a grayscale image. 

	This method is only supported on the EXYNOS8895 Device.

	@startparams
	@par{ src , Source 8-bit single channel image.} 
	@exypar{ Only supports CV_8UC1. }
	@par{ dst , Destination image of the same size and type as src . } 
	@endparams 

	Please refer to equivalent cv function.
	@see cv::equalizeHist
	*/
	CV_EXPORTS_W void equalizeHist(InputArray src,OutputArray dst);
	
	/** @brief Calculates the integral of an image. 

	This method is only supported on the EXYNOS8895 Device.

	@startparams
	@par{ src , src input image as \f$W \times H\f$\, 8-bit or floating-point (32f or 64f).} 
	@exypar{ Only supports CV_8UC1. }
	@par{ sum , integral image as \f$(W+1)\times (H+1)\f$ \, 32-bit integer or floating-point (32f or 64f).} 
	@exypar{ Only supports CV_32S. }
	@par{ sdepth , desired depth of the integral and the tilted integral images\, CV_32S\, CV_32F\, or
	CV_64F.}
	@endparams 

	Please refer to equivalent cv function.
	@see cv::integral
	*/
	CV_EXPORTS_W void integral(InputArray src, OutputArray sum, int sdepth = -1);

	/** @brief Blurs an image and downsamples it. 

	@startparams
	@par{ src , input image.} 
	@exypar{ Only supports 8UC1.\n 
	Both width and height has to be multiple of two. }
	@hexpar{ 128 Byte alignment needed.\n
	Only supports 8UC1, 16UC1, 16SC1 and 32FC1. }
	@par{ dst , output image; it has the specified size and the same type as src.  } 
	@par{ dstsize , size of the output image. } 
	@par{ borderType , Pixel extrapolation method\, see cv::BorderTypes (BORDER_CONSTANT isn't supported)}
	@exypar{ Only supports BORDER_REPLICATE. } 
	@hexpar{ Only supports BORDER_REFLECT_101. } 
	@endparams
	
	Please refer to equivalent cv function.
	@see cv::pyrDown
	*/
	CV_EXPORTS_W void pyrDown(InputArray src, OutputArray dst,
	    const Size& dstsize = Size(), int borderType = BORDER_DEFAULT);

	/** @brief Upsamples an image and then blurs it. 

	This method is only supported on the Qualcomm Device. 
	
	@startparams
	@par{ src , input image.} 
	@hexpar{ 128 Byte alignment needed.\n
	Only supports 8UC1, 16UC1, 16SC1 and 32FC1. }
	@par{ dst , output image; it has the specified size and the same type as src.  } 
	@par{ dstsize , size of the output image. } 
	@par{ borderType , Pixel extrapolation method\, see cv::BorderTypes (only BORDER_DEFAULT is supported)}
	@hexpar{ Only supports BORDER_REFLECT_101. } 
	@endparams

	Please refer to equivalent cv function.
	@see cv::pyrUp
	*/ 
	CV_EXPORTS_W void pyrUp(InputArray src, OutputArray dst,
	    const Size& dstsize = Size(), int borderType = BORDER_DEFAULT);

	/** @brief Convolves an image with the kernel. 

	This method is only supported on the EXYNOS8895 Device.

	@startparams
	@par{ src , input image } 
	@exypar{ Only supports CV_8UC1. }
	@par{ dst , output image of the same size and the same number of channels as src. } 
	@exypar{ Only supports CV_16SC1. }
	@par{ ddepth , desired depth of the destination image\, see @ref filter_depths "combinations" }
	@par{ kernel , convolution kernel (or rather a correlation kernel)\, a single-channel floating point
	matrix; if you want to apply different kernels to different channels\, split the image into
	separate color planes using split and process them individually.}
	@exypar{ Only supports kernel as 3x3, 5x5, and 7x7. }
	@par{ anchor , anchor of the kernel that indicates the relative position of a filtered point within
	the kernel; the anchor should lie within the kernel; default value (-1\,-1) means that the anchor
	is at the kernel center.}
	@exypar{Only supports default value\, (-1\, -1).}
	@par{ delta , optional value added to the filtered pixels before storing them in dst.  }
	@exypar{ Only supports delta as zero. }
	@par{ borderType , pixel extrapolation method\, see cv::BorderTypes }
	@exypar{ Only supports BORDER_REPLICATE. }
	@endparams 

	  Please refer to equivalent cv function.
	  @see cv::filter2D
	*/	
	CV_EXPORTS_W void filter2D(InputArray src, OutputArray dst, int ddepth,
		InputArray kernel, Point anchor = Point(-1, -1), 
		double delta = 0, int borderType = BORDER_DEFAULT);

	/** @brief Canny, but will update doxy later.
	*/
    CV_EXPORTS_W void Canny(InputArray _src, OutputArray _dst, double low_thresh, double high_thresh,
                int aperture_size = 3, bool L2gradient = false);


}}

	
#endif // __OPENCV_DSPIMGPROC_HPP__

// vim: noexpandtab :
