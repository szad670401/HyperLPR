//
// Created by 庾金科 on 20/09/2017.
//

#ifndef SWIFTPR_PLATEDETECTION_H
#define SWIFTPR_PLATEDETECTION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "PlateInfo.h"

namespace pr{
    class PlateDetection{
    public:
        PlateDetection(std::string filename_cascade);
        PlateDetection();
        void LoadModel(std::string filename_cascade);
        void plateDetectionRough(cv::Mat InputImage,std::vector<pr::PlateInfo>  &plateInfos,int min_w=36,int max_w=800);
//        std::vector<pr::PlateInfo> plateDetectionRough(cv::Mat InputImage,int min_w= 60,int max_h = 400);


//        std::vector<pr::PlateInfo> plateDetectionRoughByMultiScaleEdge(cv::Mat InputImage);



    public:
        cv::CascadeClassifier cascade;


    };

}// namespace pr

#endif //SWIFTPR_PLATEDETECTION_H
