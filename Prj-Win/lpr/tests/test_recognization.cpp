//
// Created by Jack Yu on 23/10/2017.
//

#include "../include/CNNRecognizer.h"

std::vector<std::string> chars{"京","沪","津","渝","冀","晋","蒙","辽","吉","黑","苏","浙","皖","闽","赣","鲁","豫","鄂","湘","粤","桂","琼","川","贵","云","藏","陕","甘","青","宁","新","0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","J","K","L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z"};

#include <opencv2/dnn.hpp>
using namespace cv::dnn;


void getMaxClass(cv::Mat &probBlob, int *classId, double *classProb)
{
//    cv::Mat probMat = probBlob.matRefConst().reshape(1, 1); //reshape the blob to 1x1000 matrix
    cv::Point classNumber;

    cv::minMaxLoc(probBlob, NULL, classProb, NULL, &classNumber);

    *classId = classNumber.x;
}

void TEST_RECOGNIZATION(){
//        pr::CNNRecognizer instance("model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel");
         Net net  = cv::dnn::readNetFromCaffe("model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel");
    cv::Mat image = cv::imread("res/char1.png",cv::IMREAD_GRAYSCALE);
    cv::resize(image,image,cv::Size(14,30));
    cv::equalizeHist(image,image);
    cv::Mat inputBlob = cv::dnn::blobFromImage(image, 1/255.0, cv::Size(14,30), false);

    net.setInput(inputBlob,"data");

    cv::Mat res = net.forward();
    std::cout<<res<<std::endl;
    float *p = (float*)res.data;
    int maxid= 0;
    double prob = 0;

    getMaxClass(res,&maxid,&prob);



        std::cout<<chars[maxid]<<std::endl;





};
int main()
{TEST_RECOGNIZATION();


}
