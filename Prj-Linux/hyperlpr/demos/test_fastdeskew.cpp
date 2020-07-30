//
// Created by Jack Yu on 02/10/2017.
//


#include <../include/FastDeskew.h>


void drawRect(cv::Mat image,cv::Rect rect)
{
    cv::Point p1(rect.x,rect.y);
    cv::Point p2(rect.x+rect.width,rect.y+rect.height);
    cv::rectangle(image,p1,p2,cv::Scalar(0,255,0),1);
}
void TEST_DESKEW(){

    cv::Mat image = cv::imread("res/3.png",cv::IMREAD_GRAYSCALE);
//    cv::resize(image,image,cv::Size(136*2,36*2));
    cv::Mat deskewed = pr::fastdeskew(image,12);
//    cv::imwrite("./res/4.png",deskewed);
//     cv::Mat deskewed2 = pr::fastdeskew(deskewed,12);
//
    cv::imshow("image",deskewed);
    cv::waitKey(0);

}
int main()
{

    TEST_DESKEW();
    return 0 ;


}
