//
// Created by 庾金科 on 21/10/2017.
//

#include "../include/CNNRecognizer.h"

namespace pr{
    CNNRecognizer::CNNRecognizer(std::string prototxt,std::string caffemodel){
        net = cv::dnn::readNetFromCaffe(prototxt, caffemodel);
    }

    label CNNRecognizer::recognizeCharacter(cv::Mat charImage){
        if(charImage.channels()== 3)
            cv::cvtColor(charImage,charImage,cv::COLOR_BGR2GRAY);
        cv::Mat inputBlob = cv::dnn::blobFromImage(charImage, 1/255.0, cv::Size(CHAR_INPUT_W,CHAR_INPUT_H), cv::Scalar(0,0,0),false);
        net.setInput(inputBlob,"data");
        return net.forward();
    }
}