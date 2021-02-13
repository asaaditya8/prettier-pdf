#include <iostream>
#include "opencv4/opencv2/opencv_modules.hpp"
#include "opencv4/opencv2/opencv.hpp"
#include "opencv4/opencv2/core.hpp"

int main (int argc, char* argv[])
{
    try
    {
        cv::Mat src_host = cv::imread("file.png", CV_LOAD_IMAGE_GRAYSCALE);

        cv::Mat result_host;


    }
    catch(const cv::Exception& ex)
    {
        std::cout << "Error: " << ex.what() << std::endl;
    }
    return 0;
}