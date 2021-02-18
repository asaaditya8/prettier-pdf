//
// Created by aaditya on 13/02/21.
//

#include "opencvhelper.h"

cv::Mat imageToMat8(const QImage &image)
{
    QImage img = image.convertToFormat(QImage::Format_RGB32).rgbSwapped();
    cv::Mat tmp(img.height(), img.width(), CV_8UC4, (void *) img.bits(), img.bytesPerLine());
    return tmp.clone();
}

void ensureC3(cv::Mat *mat)
{
    Q_ASSERT(mat->type() == CV_8UC3 || mat->type() == CV_8UC4);
    if (mat->type() != CV_8UC3) {
        cv::Mat tmp;
        cvtColor(*mat, tmp, cv::COLOR_BGRA2BGR);
        *mat = tmp;
    }
}

QImage mat8ToImage(const cv::Mat &mat)
{
    switch (mat.type()) {
        case CV_8UC1:
        {
            QVector<QRgb> ct;
            for (int i = 0; i < 256; ++i)
                ct.append(qRgb(i, i, i));
            QImage result(mat.data, mat.cols, mat.rows, (int) mat.step, QImage::Format_Indexed8);
            result.setColorTable(ct);
            return result.copy();
        }
        case CV_8UC3:
        {
            cv::Mat tmp;
            cvtColor(mat, tmp, cv::COLOR_BGRA2BGR);
            return mat8ToImage(tmp);
        }
        case CV_8UC4:
        {
            QImage result(mat.data, mat.cols, mat.rows, (int) mat.step, QImage::Format_RGB32);
            return result.rgbSwapped();
        }
        default:
            qWarning("Unhandled Mat format %d", mat.type());
            return QImage();
    }
}

void colorThresh(cv::Mat& src, int threshold) {
    switch (src.type()) {
        case CV_8UC1:
        {
            return;
        }
        case CV_8UC3:
        {
            src.forEach<cv::Vec3b>([&threshold](cv::Vec3b &d, const int* position) -> void{
                if ( static_cast<double>(d[0] & 0xff) != 255 ) {
                    double max = std::max(std::max(static_cast<double>(d[0] & 0xff), static_cast<double>(d[1] & 0xff)),
                                          static_cast<double>(d[2] & 0xff));
                    double mean = (static_cast<double>(d[0] & 0xff) + static_cast<double>(d[1] & 0xff)
                                   + static_cast<double>(d[2] & 0xff)) / 3;

                    if (max > threshold && mean < max * 0.8) {
                        d[0] = static_cast<unsigned char>(static_cast<double>(d[0] & 0xff) * 255 / max);
                        d[1] = static_cast<unsigned char>(static_cast<double>(d[1] & 0xff) * 255 / max);
                        d[2] = static_cast<unsigned char>(static_cast<double>(d[2] & 0xff) * 255 / max);
                    } else {
                        d[0] = d[1] = d[2] = 0;
                    }
                }
            });
            return;
        }
        case CV_8UC4:
        {
            src.forEach<cv::Vec4b>([&threshold](cv::Vec4b &d, const int* position) -> void{
                if ( static_cast<double>(d[0] & 0xff) != 255 ) {
                    double max = std::max(std::max(static_cast<double>(d[0] & 0xff), static_cast<double>(d[1] & 0xff)),
                                          static_cast<double>(d[2] & 0xff));
                    double mean = (static_cast<double>(d[0] & 0xff) + static_cast<double>(d[1] & 0xff)
                                   + static_cast<double>(d[2] & 0xff)) / 3;

                    if (max > threshold && mean < max * 0.8) {
                        d[0] = static_cast<unsigned char>(static_cast<double>(d[0] & 0xff) * 255 / max);
                        d[1] = static_cast<unsigned char>(static_cast<double>(d[1] & 0xff) * 255 / max);
                        d[2] = static_cast<unsigned char>(static_cast<double>(d[2] & 0xff) * 255 / max);
                    } else {
                        d[0] = d[1] = d[2] = 0;
                    }
                }
            });
            return;
        }
        default:
            return;
    }
}

QImage documentFilter(const QImage &image) {
    cv::Mat src = imageToMat8(image);
    double colorGain = 1.5;
    double colorBias = 0;
    int colorThreshold = 110;
    cv::convertScaleAbs(src, src, colorGain, colorBias);
    cv::Mat mask(src.size(), CV_8UC1);
    cv::cvtColor(src, mask, cv::COLOR_RGBA2GRAY);

    cv::Mat copy(src.size(), CV_8UC3);
    src.copyTo(copy);

    cv::adaptiveThreshold(mask,mask,255,cv::ADAPTIVE_THRESH_MEAN_C,cv::THRESH_BINARY_INV,15,15);

    src.setTo(cv::Scalar(255,255,255));
    copy.copyTo(src,mask);

    copy.release();
    mask.release();

    // special color threshold algorithm
    colorThresh(src, colorThreshold);
    return mat8ToImage(src);
}

