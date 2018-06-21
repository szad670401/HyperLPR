//
// Created by Jack Yu on 16/10/2017.
//


#include "../include/PlateSegmentation.h"
#include "../include/CNNRecognizer.h"
#include "../include/Recognizer.h"


std::vector<std::string> chars{"京","沪","津","渝","冀","晋","蒙","辽","吉","黑","苏","浙","皖","闽","赣","鲁","豫","鄂","湘","粤","桂","琼","川","贵","云","藏","陕","甘","青","宁","新","0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","J","K","L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z"};


void TEST_SLIDINGWINDOWS_EVAL(){
    cv::Mat demo = cv::imread("res/cache/finemappingres.png");
    cv::resize(demo,demo,cv::Size(136,36));

    cv::Mat respones;
    pr::PlateSegmentation plateSegmentation("model/Segmentation.prototxt","model/Segmentation.caffemodel");
    pr::PlateInfo plate;
    plate.setPlateImage(demo);
    std::vector<cv::Rect> rects;
    plateSegmentation.segmentPlatePipline(plate,1,rects);
    plateSegmentation.ExtractRegions(plate,rects);

    pr::GeneralRecognizer *recognizer = new pr::CNNRecognizer("model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel");
    recognizer->SegmentBasedSequenceRecognition(plate);
    std::cout<<plate.decodePlateNormal(chars)<<std::endl;


    delete(recognizer);





}
int main(){

    TEST_SLIDINGWINDOWS_EVAL();

    return  0;
}
