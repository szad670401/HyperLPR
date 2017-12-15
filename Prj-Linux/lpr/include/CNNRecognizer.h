//
// Created by 庾金科 on 21/10/2017.
//

#ifndef SWIFTPR_CNNRECOGNIZER_H
#define SWIFTPR_CNNRECOGNIZER_H

#include "Recognizer.h"
namespace pr{
    class CNNRecognizer: public GeneralRecognizer{
    public:
        const int CHAR_INPUT_W = 14;
        const int CHAR_INPUT_H = 30;

        CNNRecognizer(std::string prototxt,std::string caffemodel);
        label recognizeCharacter(cv::Mat character);
    private:
        cv::dnn::Net net;

    };

}

#endif //SWIFTPR_CNNRECOGNIZER_H
