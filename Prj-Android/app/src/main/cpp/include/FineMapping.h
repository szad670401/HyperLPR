//
// Created by 庾金科 on 22/09/2017.
//

#ifndef SWIFTPR_FINEMAPPING_H
#define SWIFTPR_FINEMAPPING_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include <string>
namespace pr{
    class FineMapping{
    public:
        FineMapping();


        FineMapping(std::string prototxt,std::string caffemodel);
        static cv::Mat FineMappingVertical(cv::Mat InputProposal,int sliceNum=15,int upper=0,int lower=-50,int windows_size=17);
        cv::Mat FineMappingHorizon(cv::Mat FinedVertical,int leftPadding,int rightPadding);


    private:
        cv::dnn::Net net;

    };




}
#endif //SWIFTPR_FINEMAPPING_H
