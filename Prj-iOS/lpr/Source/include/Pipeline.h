//
// Created by 庾金科 on 22/10/2017.
//

#ifndef SWIFTPR_PIPLINE_H
#define SWIFTPR_PIPLINE_H

#include "PlateDetection.h"
#include "PlateSegmentation.h"
#include "CNNRecognizer.h"
#include "PlateInfo.h"
#include "FastDeskew.h"
#include "FineMapping.h"
#include "Recognizer.h"

namespace pr{
    class PipelinePR{
        public:
            GeneralRecognizer *generalRecognizer;
            PlateDetection *plateDetection;
            PlateSegmentation *plateSegmentation;
            FineMapping *fineMapping;
        
        
        
            PipelinePR(std::string detector_filename,
                       std::string finemapping_prototxt,std::string finemapping_caffemodel,
                       std::string segmentation_prototxt,std::string segmentation_caffemodel,
                       std::string charRecognization_proto,std::string charRecognization_caffemodel
                       );
            ~PipelinePR();



            std::vector<std::string> plateRes;
            std::vector<PlateInfo> RunPiplineAsImage(cv::Mat plateImage);







    };


}
#endif //SWIFTPR_PIPLINE_H
