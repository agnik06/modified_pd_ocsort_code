#ifndef __OPENCV_XCVTRACKER_HPP__
#define __OPENCV_XCVTRACKER_HPP__

#include "cvconfig.h"
#include "opencv2/core/base.hpp"
#include "opencv2/core.hpp"

#include <vector>

namespace xcv { namespace Tracker {

struct CV_EXPORTS Kernel
{
    int type; //0-2 linear, poynomial, gaussian
    float sigma;
    int poly_a;
    int poly_b;
};

struct CV_EXPORTS Feature
{
    int type; //0 gray, 1 hog
    bool gray;
    bool hog;
    int hog_orientations;
};


struct CV_EXPORTS Para
{
    Kernel kernel;
    Feature feature;
    float interp_factor;
    int cell_size;
    float padding; // = 1.5
    float lambda;
    float output_sigma_factor; // = 0.1f
};

class CV_EXPORTS TrackerKCFImpl
{
public:

    TrackerKCFImpl();

    ~TrackerKCFImpl();

    void release();

    void initialize(int rows, int cols, unsigned char * im, int * firstpose, int * target_size);

    bool run(int rows, int cols, unsigned char * im, bool isInit, bool isUpdate = false);

    void ndgrid(int * sz, int * rs, int * cs);

    bool getGaussianShapedLabels(float sigma, int * sz, float * labels);

    void getGaussianCorrelation(int m, int n, int channels, float * _xf_Re, float * _xf_Im, float * _yf_Re, float * _yf_Im, float sigma, float * _kf_Re, float * _kf_Im);

    void getLinearCorrelation(int m, int n, int channels, float * _xf_Re, float * _xf_Im, float * _yf_Re, float * _yf_Im, float * _kf_Re, float * _kf_Im);

    void getSubWindow(int m, int n, unsigned char * im, int * pos, int * sz, unsigned char * _patch);

    int pose[2];
    float lastResponse;

    int left;
    int top;
    int right;
    int bottom;
    int trackFlag;

private:
    Para para;
    bool resize_image;
    int window_sz[2];
    int ori_sz[2];
    int size[2];
    int cell_size;
    float * xf_Re;
    float * xf_Im;
    float * yf_Re;
    float * yf_Im;
    float * zf_Re;
    float * zf_Im;
    float * kf_Re;
    float * kf_Im;
    float * kzf_Re;
    float * kzf_Im;
    float * alphaf_Re;
    float * alphaf_Im;
    float * model_alphaf_Re;
    float * model_alphaf_Im;
    float * model_xf_Re;
    float * model_xf_Im;
    float * cos_window; //store pre-computed cosine window
    unsigned char * patch;
    int featChannels;
    float * feat; //the last channel is not used
};


class CV_EXPORTS TrackerORBMCHImpl
{
public:
    TrackerORBMCHImpl();
    ~TrackerORBMCHImpl();
    void release();

    void initialize(int rows, int cols, unsigned char * im, int * firstpose, int * firsttarget_sz);
    bool run(int rows, int cols, unsigned char * im_ori, bool isInit, bool isUpdate);

    int left;
    int top;
    int right;
    int bottom;

private:
    void get_subwindow(int m, int n, unsigned char * im, int * pos, int * sz, unsigned char * tpatch);
    int pose[2];
    int trackFlag;

    Para para;
    bool resize_image;
    int window_sz[2];
    int ori_sz[2];
    unsigned char * patch;

};

class CV_EXPORTS TrackerKCF
{
public:
	TrackerKCF();
	~TrackerKCF();

	int initialize(int rows, int cols ,unsigned char * data, int roi_left, int roi_top, int roi_width, int roi_height);
	int run(int rows, int cols, unsigned char * data, int * left, int * top, int * right, int * bottom, int * mv_x, int * mv_y);
	void release();

	struct TrackingROI {
		int left;
		int right;
		int top;
		int bottom;
	};

	TrackingROI m_previous_roi;

private:
	int m_nWidth, m_nHeight;
	void * tracker;
};

CV_EXPORTS_W void calcForwardBackwardOpticalFlowLK(const std::vector<cv::Mat> &prevPyr,
        const std::vector<cv::Mat> &currPyr,
        const std::vector<cv::Point2f> &pts,
        std::vector<cv::Point2f> &fwdPts,
        std::vector<cv::Point2f> &backPts,
        std::vector<uint8_t> &status,
        std::vector<uint8_t> &statusBack);
}}


#endif // __OPENCV_XCVTRACKER_HPP__

// vim: noexpandtab :
