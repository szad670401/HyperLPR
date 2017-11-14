//
// Created by 庾金科 on 04/04/2017.
//

#include <opencv2/opencv.hpp>

namespace util{

    template <class T> void swap ( T& a, T& b )
    {
        T c(a); a=b; b=c;
    }

    template <class T> T min(T& a,T& b )
    {
        return a>b?b:a;

    }

    cv::Mat cropFromImage(const cv::Mat &image,cv::Rect rect){
        int w = image.cols-1;
        int h = image.rows-1;
        rect.x = std::max(rect.x,0);
        rect.y = std::max(rect.y,0);
        rect.height = std::min(rect.height,h-rect.y);
        rect.width = std::min(rect.width,w-rect.x);
        cv::Mat temp(rect.size(), image.type());
        cv::Mat cropped;
        temp = image(rect);
        temp.copyTo(cropped);
        return cropped;

    }

    cv::Mat cropBox2dFromImage(const cv::Mat &image,cv::RotatedRect rect)
    {
        cv::Mat M, rotated, cropped;
        float angle = rect.angle;
        cv::Size rect_size(rect.size.width,rect.size.height);
        if (rect.angle < -45.) {
            angle += 90.0;
            swap(rect_size.width, rect_size.height);
        }
        M = cv::getRotationMatrix2D(rect.center, angle, 1.0);
        cv::warpAffine(image, rotated, M, image.size(), cv::INTER_CUBIC);
        cv::getRectSubPix(rotated, rect_size, rect.center, cropped);
        return cropped;
    }

    cv::Mat calcHist(const cv::Mat &image)
    {
        cv::Mat hsv;
        std::vector<cv::Mat> hsv_planes;
        cv::cvtColor(image,hsv,cv::COLOR_BGR2HSV);
        cv::split(hsv,hsv_planes);
        cv::Mat hist;
        int histSize = 256;
        float range[] = {0,255};
        const float* histRange = {range};

        cv::calcHist( &hsv_planes[0], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange,true, true);
        return hist;

    }
    float computeSimilir(const cv::Mat &A,const cv::Mat &B)
    {

        cv::Mat histA,histB;
        histA = calcHist(A);
        histB = calcHist(B);
        return cv::compareHist(histA,histB,CV_COMP_CORREL);

    }





}//namespace util
