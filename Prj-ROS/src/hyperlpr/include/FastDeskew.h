//
// Created by Jack Yu on 22/09/2017.
//

#ifndef HYPERPR_FASTDESKEW_H
#define HYPERPR_FASTDESKEW_H

#include <math.h>
#include <opencv2/opencv.hpp>
namespace pr {

cv::Mat fastdeskew(cv::Mat skewImage, int blockSize);
//    cv::Mat spatialTransformer(cv::Mat skewImage);

} // namespace pr

#endif // HYPERPR_FASTDESKEW_H
