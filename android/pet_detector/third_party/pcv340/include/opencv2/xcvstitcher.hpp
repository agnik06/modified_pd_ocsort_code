#ifndef __OPENCV_XCVSTITCHER_HPP__
#define __OPENCV_XCVSTITCHER_HPP__

#include "cvconfig.h"
#include "opencv2/core/base.hpp"
#include "opencv2/core.hpp"
#include <vector>

using namespace std;
using namespace cv;

namespace xcv { namespace Stitcher {

class CV_EXPORTS Blender
{
public:
	enum { NO, FEATHER, MULTIBAND };

	virtual ~Blender() {}

	static Ptr<Blender> createDefault(int type);

	void prepare(const std::vector<Point> &corners, const std::vector<Size> &sizes);

	void prepareOverlapping(const Point corners, const Size sizes);

	virtual void prepare(Rect dst_roi);

	virtual void feed(const Mat &img, const Mat &mask, Point tl);

	virtual void blend(Mat &dst, Mat &dst_mask);

protected:
	cv::Mat dst_, dst_mask_;
	cv::Rect dst_roi_;
};

class CV_EXPORTS MultiBandBlender : public Blender
{
public:
	MultiBandBlender(int numberOfBands = 5, int type = CV_16S);

	int getNumBands() const { return actual_num_bands_; }

	void setNumBands(int val) { actual_num_bands_ = val; }

	void prepare(Rect dst_roi);

	void feed(const Mat &img, const Mat &mask, Point tl);

	void blend(Mat &dst, Mat &dst_mask);

	int num_images;
	int blend_offset;

private:
	int actual_num_bands_;
	int num_bands_;
	std::vector<Mat> dst_pyr_laplace_;
	std::vector<Mat> dst_band_weights_;
	Rect dst_roi_final_;
	int weight_type_;
};


class CV_EXPORTS WarperBase
{
public:

	virtual ~WarperBase() {}

	virtual Point warpPoint(const Point2f &pt, const Mat &K, const Mat &R) = 0;

	virtual Rect buildMaps(Size src_size, const Mat &K, const Mat &R, Mat &xmap, Mat &ymap) = 0;

	virtual void mapForward(float x, float y, float &u, float &v) = 0;

	virtual void mapBackward(int s32xloc, int s32yloc, float &u, float &v) = 0;

	virtual Point warp(const Mat &src, const Mat &K, const Mat &R, int interp_mode, int border_mode, Mat &dst) = 0;

	virtual void warpLinear(const Mat &src, const Rect dstROI, const Mat &K, const Mat &R, Mat &dst, Mat &dst_mask, Mat &xmap, Mat &ymap) = 0;

	Rect warpRoi(Size src_size, const Mat &K, const Mat &R);

	Rect warpRoi_small(Size src_size, const Mat &K, const Mat &R);

	void setCameraParams(const Mat &K = Mat::eye(3, 3, CV_32F), const Mat &R = Mat::eye(3, 3, CV_32F), const Mat &T = Mat::zeros(3, 1, CV_32F));

public:
	float fScale;
	float afk[9];
	float afrinv[9];
	float afr_kinv[9];
	float afk_rinv[9];
	float aft[3];
	float *pfsineu;
	float *pfsinev;
	float *pfcosineu;
	float *pfcosinev;
	float *pfv;

protected:

	virtual void detectResultRoi(Size src_size, Point &dst_tl, Point &dst_br) = 0;

	virtual void detectResultRoi_small(Size src_size, Point &dst_tl, Point &dst_br) = 0;

};


class CV_EXPORTS CylindricalWarper : public WarperBase
{
public:

	CylindricalWarper(float scale);

	Point warpPoint(const Point2f &pt, const Mat &K, const Mat &R);

	Rect buildMaps(Size src_size, const Mat &K, const Mat &R, Mat &xmap, Mat &ymap);

	void warpLinear(const Mat &src, const Rect dstROI, const Mat &K, const Mat &R, Mat &dst, Mat &dst_mask, Mat& xmap, Mat& ymap);

	void mapForward(float x, float y, float &u, float &v);

	void mapBackward(int s32xloc, int s32v, float &x, float &y);

	Point warp(const Mat &src, const Mat &K, const Mat &R, int interp_mode, int border_mode, Mat &dst);

protected:

	void detectResultRoi(Size src_size, Point &dst_tl, Point &dst_br);

	void detectResultRoi_small(Size src_size, Point &dst_tl, Point &dst_br);

	void detectResultRoiByBorder_small(Size src_size, Point &dst_tl, Point &dst_br);

private:

	static void *warper_kernel(void*);
};


}}


#endif // __OPENCV_XCVSTITCHER_HPP__

// vim: noexpandtab :
