//
// Created by aaditya on 13/02/21.
//

#ifndef WASH_OPENCVHELPER_H
#define WASH_OPENCVHELPER_H

#include <opencv2/opencv.hpp>
#include <QtGui/QImage>

// QImage -> CV_8UC4
cv::Mat imageToMat8(const QImage &image);

// CV_8UC3|4 -> CV_8UC3
void ensureC3(cv::Mat *mat);

// CV_8UC1|3|4 -> QImage
QImage mat8ToImage(const cv::Mat &mat);

void colorThresh(cv::Mat& src, int threshold);

QImage documentFilter(const QImage &image);

#endif //WASH_OPENCVHELPER_H
