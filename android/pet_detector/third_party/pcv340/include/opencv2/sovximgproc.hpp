#ifndef __OPENCV_SOVXIMGPROC_HPP__
#define __OPENCV_SOVXIMGPROC_HPP__

#include "cvconfig.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"


namespace cv
{
	namespace sovx
	{

		/** @brief Blurs an image using the box filter.
		
		The function smoothes an image using the kernel:
		Unnormalized box filter is useful for computing various integral characteristics over each pixel
		neighborhood, such as covariance matrices of image derivatives (used in dense optical flow
		algorithms, and so on). If you need to compute pixel sums over variable-size windows, use cv::integral.
		
		@param src input image.It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst output image of the same size and type as src.
		@param ddepth the output image depth (-1 to use src.depth()).
		@param ksize blurring kernel size. it should be Size(3, 3).
		@param anchor anchor point; default value Point(-1,-1) means that the anchor is at the kernel center. It should be Point(-1, -1) or Point(1, 1)
		@param normalize flag, specifying whether the kernel is normalized by its area or not.
		@param borderType border mode used to extrapolate pixels outside of the image, see cv::BorderTypes
		@sa  blur, bilateralFilter, GaussianBlur, medianBlur, integral
		*/
		CV_EXPORTS_W void boxFilter(InputArray src, OutputArray dst, int ddepth = -1, Size ksize = Size(3, 3), Point anchor = Point(-1, -1), bool normalize = true, int borderType = BORDER_REPLICATE);

		/** @brief Blurs an image using the normalized box filter.
		
		The function smoothes an image using the kernel:
		The call `blur(src, dst, ksize, anchor, borderType)` is equivalent to `boxFilter(src, dst, src.type(), anchor, true, borderType)`.
		
		@param src input image; It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst output image of the same size and type as src.
		@param ksize blurring kernel size. It should be Size(3, 3).
		@param anchor anchor point; default value Point(-1,-1) means that the anchor is at the kernel center. It should be Point(-1, -1) or Point(1, 1)
		@param borderType border mode used to extrapolate pixels outside of the image, see cv::BorderTypes
		@sa  boxFilter, GaussianBlur
		*/
		CV_EXPORTS_W void blur(InputArray src, OutputArray dst, Size ksize = Size(3, 3), Point anchor = Point(-1, -1), int borderType = BORDER_REPLICATE);

		/** @brief Blurs an image using a Gaussian filter.
		
		The function convolves the source image with the specified Gaussian kernel. In-place filtering is
		supported.
		
		@param src input image; It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst output image of the same size and type as src.
		@param ksize Gaussian kernel size. ksize.width and ksize.height both must be positive and odd. Or, they can be zero's and then they are computed from sigma. Butit should be Size(3, 3) or Size(5, 5).
		@param sigmaX Gaussian kernel standard deviation in X direction. It should be zero, sigmaX == 0.
		@param sigmaY Gaussian kernel standard deviation in Y direction; It should be zero, sigmaY == 0.
		if sigmaY is zero, it is set to be equal to sigmaX, if both sigmas are zeros, they are computed from ksize.width and ksize.height,
		respectively (see cv::getGaussianKernel for details); to fully control the result regardless of possible future modifications of all this semantics, it is recommended to specify all of ksize, sigmaX, and sigmaY.
		@param borderType pixel extrapolation method, see cv::BorderTypes
		
		@sa  sepFilter2D, filter2D, blur, boxFilter, bilateralFilter, medianBlur
		*/
		CV_EXPORTS_W void GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX = 0, double sigmaY = 0, int borderType = BORDER_REPLICATE);

		/** @brief Resizes an image.
		
		The function resize resizes the image src down to or up to the specified size. Note that the
		initial dst type or size are not taken into account. Instead, the size and type are derived from
		the `src`,`dsize`,`fx`, and `fy`. If you want to resize src so that it fits the pre-created dst,
		you may call the function as follows:
		@code
			// explicitly specify dsize=dst.size(); fx and fy will be computed from that.
		   cv::sovx::resize(src, dst, dst.size(), 0, 0, interpolation);
		@endcode
		If you want to decimate the image by factor of 2 in each direction, you can call the function this
		way:
		@code
			// specify fx and fy and let the function compute the destination image size.
			cv::sovx::resize(src, dst, Size(), 0.5, 0.5, interpolation);
		@endcode
		To shrink an image, it will generally look best with CV_INTER_AREA interpolation, whereas to
		enlarge an image, it will generally look best with CV_INTER_CUBIC (slow) or CV_INTER_LINEAR
		(faster but still looks OK).
		
		@param src input image. It should be CV_8UC1, CV_8UC3 or CV_8UC4.
		@param dst output image; it has the size dsize (when it is non-zero) or the size computed from src.size(), fx, and fy; the type of dst is the same as of src.
		@param dSize output image size; if it equals zero, it is computed as:
		 \f[\texttt{dsize = Size(round(fx*src.cols), round(fy*src.rows))}\f]
		 Either dsize or both fx and fy must be non-zero.
		@param fx scale factor along the horizontal axis; fx and fy should be identical. Also it should be in range fx >= 0.25 && fx <= 8.
		@param fy scale factor along the vertical axis;
		@param interpolation interpolation method, see cv::InterpolationFlags INTER_LINEAR or INTER_CUBIC.
		
		@sa  warpAffine, warpPerspective, remap
		*/
		CV_EXPORTS_W void resize(InputArray src, OutputArray dst, Size dSize, double fx = 0, double fy = 0, int interpolation = INTER_LINEAR);

		/** @brief Blurs an image and downsamples it.
		
		By default, size of the output image is computed as `Size((src.cols+1)/2, (src.rows+1)/2)`.
		The function performs the downsampling step of the Gaussian pyramid construction.
		Then, it downsamples the image by rejecting even rows and columns.
		
		@param src input image. It should be CV_8UC1, CV_8UC3 and CV_8UC4. image width and heigh lager than zero.
		@param dst output image; it has the specified size and the same type as src.
		@param dstsize size of the output image. It should be zero or `Size((src.cols+1)/2, (src.rows+1)/2)`.
		@param borderType Pixel extrapolation method, see cv::BorderTypes (only BORDER_REPLICATE supported)
		*/
		CV_EXPORTS_W void pyrDown(InputArray src, OutputArray dst, const Size& dstsize = Size(), int borderType = BORDER_REPLICATE);

		/** @brief Upsamples an image and then blurs it.
		
		By default, size of the output image is computed as `Size(src.cols\*2, (src.rows\*2)`.
		The function performs the upsampling step of the Gaussian pyramid construction, though it can
		actually be used to construct the Laplacian pyramid. First, it upsamples the source image by
		injecting even zero rows and columns and then convolves the result with the same kernel as in pyrDown multiplied by 4.
		
		@param src input image. It should be CV_8UC1, CV_8UC3 and CV_8UC4. Image width and heigh lager than zero.
		@param dst output image. It has the specified size and the same type as src .
		@param dstsize size of the output image.It should be zero or `Size((src.cols+1)/2, (src.rows+1)/2)`.
		@param borderType Pixel extrapolation method, see cv::BorderTypes (only BORDER_REPLICATE supported)
		 */
		CV_EXPORTS_W void pyrUp(InputArray src, OutputArray dst, const Size& dstsize = Size(), int borderType = BORDER_REPLICATE);

		/** @brief Constructs the Gaussian pyramid for an image.
		
		The function constructs a vector of images and builds the Gaussian pyramid by recursively applying
		pyrDown to the previously built pyramid layers, starting from `dst[0]==src`.
		
		@param src Source image. It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst Destination vector of maxlevel+1 images of the same type as src. dst[0] will be the same as src. dst[1] is the next pyramid layer, a smoothed and down-sized src, and so on.
		@param maxlevel 0-based index of the last (the smallest) pyramid layer. It must be non-negative.
		@param borderType Pixel extrapolation method, see cv::BorderTypes (BORDER_CONSTANT isn't supported)
		 */
		CV_EXPORTS_W void buildPyramid(InputArray src, OutputArrayOfArrays dst, int maxlevel, int borderType = BORDER_REPLICATE);

		/** @brief Harris corner detector.
		
		The function runs the Harris corner detector on the image then nomalize output reponse. 
		Corners in the image can be found as the local maxima of this response map.
		
		@param src Input single-channel 8-bit image. It support only CV_8UC1.
		@param dst Image to store the Harris detector responses. It has the type CV_32FC1 and the same size as src .
		@param blockSize Neighborhood size. It should be 3,5,7,9,11,13 and 15.
		@param ksize Aperture parameter for the Sobel operator. It onlu support ksize = 3.
		@param k Harris detector free parameter.It should be ( k > 0.01 && k < 0.1 ).
		@param borderType Pixel extrapolation method. See cv::BorderTypes. ( BORDER_CONSTANT or BORDER_REPLICATE is supported);
		 */
		CV_EXPORTS_W void cornerHarris_norm(InputArray src, OutputArray dst, int blockSize, int ksize=3, double k=0.04, int borderType = BORDER_REPLICATE);

		/** @brief Applies a perspective transformation to an image.
		
		The function warpPerspective transforms the source image using the specified matrix when the flag WARP_INVERSE_MAP is set. 
		Otherwise, the transformation is first inverted with invert and then put in the formula above instead of M. 
		The function cannot operate in-place.
		
		@param src input image. It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst output image that has the size dsize and the same type as src .
		@param M \f$3\times 3\f$ transformation matrix.
		@param dsize size of the output image.
		@param flags combination of interpolation methods (see cv::InterpolationFlags(INTER_LINEAR, INTER_CUBIC, INTER_NEAREST)) and the optional flag WARP_INVERSE_MAP, that sets M as the inverse transformation (\f$\texttt{dst}\rightarrow\texttt{src}\f$ ).
		@param borderMode pixel extrapolation method (BORDER_CONSTANT).
		@param borderValue value used in case of a constant border; by default, it equals 0.
		
		@sa  warpAffine, resize, remap, getRectSubPix, perspectiveTransform
		*/
		CV_EXPORTS_W void warpPerspective(InputArray src, OutputArray dst, InputArray M, Size dsize, int flags = INTER_LINEAR, int borderMode = BORDER_CONSTANT, const Scalar& borderValue = Scalar());

		/** @brief Applies an affine transformation to an image.
		
		The function warpAffine transforms the source image using the specified matrix, 
		when the flag WARP_INVERSE_MAP is set. Otherwise, the transformation is first inverted
		with cv::invertAffineTransform and then put in the formula above instead of M. 
		The function cannot operate in-place.
		
		@param src input image. It should be CV_8UC1, CV_8UC3 and CV_8UC4.
		@param dst output image that has the size dsize and the same type as src .
		@param M \f$2\times 3\f$ transformation matrix.
		@param dSize size of the output image.
		@param flags combination of interpolation methods (see cv::InterpolationFlags(INTER_LINEAR, INTER_CUBIC, INTER_NEAREST)) and the optional flag WARP_INVERSE_MAP that means that M is the inverse transformation (\f$\texttt{dst}\rightarrow\texttt{src}\f$ ).
		@param borderMode pixel extrapolation method (see cv::BorderTypes, BORDER_CONSTANT); 
		@param borderValue value used constant border; by default, it is Scalar::all(0).
		
		@sa  warpPerspective, resize, remap, getRectSubPix, transform
		 */
		CV_EXPORTS_W void warpAffine(InputArray src, OutputArray dst, InputArray M, Size dSize, int flags = INTER_LINEAR, int borderMode = BORDER_CONSTANT, const Scalar& borderValue = Scalar());

		Mat IVAgetAffineTransform(Point2f input[], Point2f output[]);

		CV_EXPORTS_W void remap(InputArray src, OutputArray dst, InputArray map1, InputArray map2, int interpolation, int borderMode = BORDER_REPLICATE, const Scalar& borderValue = Scalar());
	}
}

#endif // __OPENCV_SOVXIMGPROC_HPP__
