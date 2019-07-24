//
// Created by 庾金科 on 02/10/2017.
//



#include "../include/FastDeskew.h"
#include <cmath>
#include <vector>
#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

namespace pr{


    const int  ANGLE_MIN = 30 ;
    const int ANGLE_MAX = 150 ;
    const int PLATE_H = 36;
    const int PLATE_W = 136;

    int angle(float x,float y)
    {
        return atan2(x,y)*180/3.1415;
    }

    std::vector<float> avgfilter(std::vector<float> angle_list,int windowsSize) {
        std::vector<float> angle_list_filtered(angle_list.size() - windowsSize + 1);
        for (int i = 0; i < angle_list.size() - windowsSize + 1; i++) {
            float avg = 0.00f;
            for (int j = 0; j < windowsSize; j++) {
                avg += angle_list[i + j];
            }
            avg = avg / windowsSize;
            angle_list_filtered[i] = avg;
        }

        return angle_list_filtered;
    }


    void drawHist(std::vector<float> seq){
        cv::Mat image(300,seq.size(),CV_8U);
        image.setTo(0);

        for(int i = 0;i<seq.size();i++)
        {
            float l = *std::max_element(seq.begin(),seq.end());

            int p = int(float(seq[i])/l*300);

            cv::line(image,cv::Point(i,300),cv::Point(i,300-p),cv::Scalar(255,255,255));
        }
        cv::imshow("vis",image);
    }

    cv::Mat  correctPlateImage(cv::Mat skewPlate,float angle,float maxAngle)
    {

        cv::Mat dst;

        cv::Size size_o(skewPlate.cols,skewPlate.rows);


        int extend_padding = 0;
//        if(angle<0)
            extend_padding = static_cast<int>(skewPlate.rows*tan(cv::abs(angle)/180* 3.14) );
//        else
//            extend_padding = static_cast<int>(skewPlate.rows/tan(cv::abs(angle)/180* 3.14) );

//        std::cout<<"extend:"<<extend_padding<<std::endl;

        cv::Size size(skewPlate.cols + extend_padding ,skewPlate.rows);

        float interval = std::abs(sin((angle /180) * 3.14)* skewPlate.rows);
//        std::cout<<interval<<std::endl;

        cv::Point2f pts1[4] = {cv::Point2f(0,0),cv::Point2f(0,size_o.height),cv::Point2f(size_o.width,0),cv::Point2f(size_o.width,size_o.height)};
        if(angle>0) {
            cv::Point2f pts2[4] = {cv::Point2f(interval, 0), cv::Point2f(0, size_o.height),
                                   cv::Point2f(size_o.width, 0), cv::Point2f(size_o.width - interval, size_o.height)};
            cv::Mat M  = cv::getPerspectiveTransform(pts1,pts2);
            cv::warpPerspective(skewPlate,dst,M,size);


        }
        else {
            cv::Point2f pts2[4] = {cv::Point2f(0, 0), cv::Point2f(interval, size_o.height), cv::Point2f(size_o.width-interval, 0),
                                   cv::Point2f(size_o.width, size_o.height)};
            cv::Mat M  = cv::getPerspectiveTransform(pts1,pts2);
            cv::warpPerspective(skewPlate,dst,M,size,cv::INTER_CUBIC);

        }
        return  dst;
    }
    cv::Mat fastdeskew(cv::Mat skewImage,int blockSize){


        const int FILTER_WINDOWS_SIZE = 5;
        std::vector<float> angle_list(180);
        memset(angle_list.data(),0,angle_list.size()*sizeof(int));

        cv::Mat bak;
        skewImage.copyTo(bak);
        if(skewImage.channels() == 3)
            cv::cvtColor(skewImage,skewImage,cv::COLOR_RGB2GRAY);

        if(skewImage.channels() == 1)
        {
            cv::Mat eigen;

            cv::cornerEigenValsAndVecs(skewImage,eigen,blockSize,5);
            for( int j = 0; j < skewImage.rows; j+=blockSize )
            { for( int i = 0; i < skewImage.cols; i+=blockSize )
                {
                    float x2 = eigen.at<cv::Vec6f>(j, i)[4];
                    float y2 = eigen.at<cv::Vec6f>(j, i)[5];
                    int angle_cell = angle(x2,y2);
                    angle_list[(angle_cell + 180)%180]+=1.0;

                }
            }
        }
        std::vector<float> filtered = avgfilter(angle_list,5);

        int maxPos = std::max_element(filtered.begin(),filtered.end()) - filtered.begin() + FILTER_WINDOWS_SIZE/2;
        if(maxPos>ANGLE_MAX)
            maxPos = (-maxPos+90+180)%180;
        if(maxPos<ANGLE_MIN)
            maxPos-=90;
        maxPos=90-maxPos;
      cv::Mat deskewed = correctPlateImage(bak, static_cast<float>(maxPos),60.0f);
        return deskewed;
    }



}//namespace pr
