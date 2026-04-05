#ifndef UTILS_H
#define UTILS_H

#include <opencv2/core.hpp>
#include <vector>
#include "stats.h"

using namespace std;
using namespace cv;

void drawBoundingBox(cv::Mat image, std::vector<Point2f> bb);
void drawStatistics(cv::Mat image, const Stats& stats);
void printStatistics(std::string name, Stats stats);
std::vector<cv::Point2f> Points(std::vector<cv::KeyPoint> keypoints);

void drawBoundingBox(cv::Mat image,std:: vector<Point2f> bb)
{
    for(unsigned i = 0; i < bb.size() - 1; i++) {
        line(image, bb[i], bb[i + 1], cv::Scalar(0, 0, 255), 2);
    }
    line(image, bb[bb.size() - 1], bb[0], cv::Scalar(0, 0, 255), 2);
}

void drawStatistics(cv::Mat image, const Stats& stats)
{
    static const int font = FONT_HERSHEY_PLAIN;
    std::stringstream str1, str2, str3;

    str1 << "Matches: " << stats.matches;
    str2 << "Inliers: " << stats.inliers;
    str3 << "Inlier ratio: " << std::setprecision(2) << stats.ratio;

    putText(image, str1.str(), cv::Point(0, image.rows - 90), font, 2, cv::Scalar::all(255), 3);
    putText(image, str2.str(), cv::Point(0, image.rows - 60), font, 2, cv::Scalar::all(255), 3);
    putText(image, str3.str(), cv::Point(0, image.rows - 30), font, 2, cv::Scalar::all(255), 3);
}

void printStatistics(std::string name, Stats stats)
{
    std::cout << name << endl;
    std::cout << "----------" << endl;

    std::cout << "Matches " << stats.matches << endl;
    std::cout << "Inliers " << stats.inliers << endl;
    std::cout << "Inlier ratio " << setprecision(2) << stats.ratio << endl;
    std::cout << "Keypoints " << stats.keypoints << endl;
    std::cout << endl;
}

std::vector<cv::Point2f> Points(std::vector<cv::KeyPoint> keypoints)
{
    std::vector<cv::Point2f> res;
    for(unsigned i = 0; i < keypoints.size(); i++) {
        res.push_back(keypoints[i].pt);
    }
    return res;
}

#endif // UTILS_H
