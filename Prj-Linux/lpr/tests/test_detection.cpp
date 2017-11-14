//
// Created by 庾金科 on 20/09/2017.
//

#include <../include/PlateDetection.h>


void drawRect(cv::Mat image,cv::Rect rect)
{
    cv::Point p1(rect.x,rect.y);
    cv::Point p2(rect.x+rect.width,rect.y+rect.height);
    cv::rectangle(image,p1,p2,cv::Scalar(0,255,0),1);
}


int main()
{
    cv::Mat image = cv::imread("res/test1.jpg");
    pr::PlateDetection plateDetection("model/cascade.xml");
    std::vector<pr::PlateInfo> plates;
    plateDetection.plateDetectionRough(image,plates);
    for(pr::PlateInfo platex:plates)
    {
        drawRect(image,platex.getPlateRect());
        cv::imwrite("res/cache/test.png",platex.getPlateImage());
        cv::imshow("image",platex.getPlateImage());
        cv::waitKey(0);
    }
    cv::imshow("image",image);
    cv::waitKey(0);
    return 0 ;


}
