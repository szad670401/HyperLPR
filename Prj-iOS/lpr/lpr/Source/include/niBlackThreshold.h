//
// Created by 庾金科 on 26/10/2017.
//

#ifndef SWIFTPR_NIBLACKTHRESHOLD_H
#define SWIFTPR_NIBLACKTHRESHOLD_H


#include <opencv2/opencv.hpp>
using namespace cv;

enum LocalBinarizationMethods{
    BINARIZATION_NIBLACK = 0, //!< Classic Niblack binarization. See @cite Niblack1985 .
    BINARIZATION_SAUVOLA = 1, //!< Sauvola's technique. See @cite Sauvola1997 .
    BINARIZATION_WOLF = 2,    //!< Wolf's technique. See @cite Wolf2004 .
    BINARIZATION_NICK = 3     //!< NICK technique. See @cite Khurshid2009 .
};


void niBlackThreshold( InputArray _src, OutputArray _dst, double maxValue,
                       int type, int blockSize, double k, int binarizationMethod )
{
    // Input grayscale image
    Mat src = _src.getMat();
    CV_Assert(src.channels() == 1);
    CV_Assert(blockSize % 2 == 1 && blockSize > 1);
    if (binarizationMethod == BINARIZATION_SAUVOLA) {
        CV_Assert(src.depth() == CV_8U);
    }
    type &= THRESH_MASK;
    // Compute local threshold (T = mean + k * stddev)
    // using mean and standard deviation in the neighborhood of each pixel
    // (intermediate calculations are done with floating-point precision)
    Mat test;
    Mat thresh;
    {
        // note that: Var[X] = E[X^2] - E[X]^2
        Mat mean, sqmean, variance, stddev, sqrtVarianceMeanSum;
        double srcMin, stddevMax;
        boxFilter(src, mean, CV_32F, Size(blockSize, blockSize),
                  Point(-1,-1), true, BORDER_REPLICATE);
        sqrBoxFilter(src, sqmean, CV_32F, Size(blockSize, blockSize),
                     Point(-1,-1), true, BORDER_REPLICATE);
        variance = sqmean - mean.mul(mean);
        sqrt(variance, stddev);
        switch (binarizationMethod)
        {
            case BINARIZATION_NIBLACK:
                thresh = mean + stddev * static_cast<float>(k);

                break;
            case BINARIZATION_SAUVOLA:
                thresh = mean.mul(1. + static_cast<float>(k) * (stddev / 128.0 - 1.));
                break;
            case BINARIZATION_WOLF:
                minMaxIdx(src, &srcMin,NULL);
                minMaxIdx(stddev, NULL, &stddevMax);
                thresh = mean - static_cast<float>(k) * (mean - srcMin - stddev.mul(mean - srcMin) / stddevMax);
                break;
            case BINARIZATION_NICK:
                sqrt(variance + sqmean, sqrtVarianceMeanSum);
                thresh = mean + static_cast<float>(k) * sqrtVarianceMeanSum;
                break;
            default:
                CV_Error( CV_StsBadArg, "Unknown binarization method" );
                break;
        }
        thresh.convertTo(thresh, src.depth());

        thresh.convertTo(test, src.depth());
//
//        cv::imshow("imagex",test);
//        cv::waitKey(0);

    }
    // Prepare output image
    _dst.create(src.size(), src.type());
    Mat dst = _dst.getMat();
    CV_Assert(src.data != dst.data);  // no inplace processing
    // Apply thresholding: ( pixel > threshold ) ? foreground : background
    Mat mask;
    switch (type)
    {
        case THRESH_BINARY:      // dst = (src > thresh) ? maxval : 0
        case THRESH_BINARY_INV:  // dst = (src > thresh) ? 0 : maxval
            compare(src, thresh, mask, (type == THRESH_BINARY ? CMP_GT : CMP_LE));
            dst.setTo(0);
            dst.setTo(maxValue, mask);
            break;
        case THRESH_TRUNC:       // dst = (src > thresh) ? thresh : src
            compare(src, thresh, mask, CMP_GT);
            src.copyTo(dst);
            thresh.copyTo(dst, mask);
            break;
        case THRESH_TOZERO:      // dst = (src > thresh) ? src : 0
        case THRESH_TOZERO_INV:  // dst = (src > thresh) ? 0 : src
            compare(src, thresh, mask, (type == THRESH_TOZERO ? CMP_GT : CMP_LE));
            dst.setTo(0);
            src.copyTo(dst, mask);
            break;
        default:
            CV_Error( CV_StsBadArg, "Unknown threshold type" );
            break;
    }
}

#endif //SWIFTPR_NIBLACKTHRESHOLD_H
