//
// Created by Jack Yu on 24/09/2017.
//

#include "FineMapping.h"






int main()
{
    cv::Mat image = cv::imread("res/cache/test.png");
    cv::Mat image_finemapping = pr::FineMapping::FineMappingVertical(image);
    pr::FineMapping finemapper =  pr::FineMapping("model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel");
     image_finemapping = finemapper.FineMappingHorizon(image_finemapping,0,-3);
    cv::imwrite("res/cache/finemappingres.png",image_finemapping);
    cv::imshow("image",image_finemapping);
    cv::waitKey(0);


    return 0 ;

}
