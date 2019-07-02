//
// Created by â×½ð¿Æ on 22/10/2017.
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
#include "SegmentationFreeRecognizer.h"

namespace pr{

    const std::vector<std::string> CH_PLATE_CODE{"¾©", "»¦", "½ò", "Óå", "¼½", "½ú", "ÃÉ", "ÁÉ", "¼ª", "ºÚ", "ËÕ", "Õã", "Íî", "Ãö", "¸Ó", "Â³", "Ô¥", "¶õ", "Ïæ", "ÔÁ", "¹ð",
                                        "Çí", "´¨", "¹ó", "ÔÆ", "²Ø", "ÉÂ", "¸Ê", "Çà", "Äþ", "ÐÂ", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
                                        "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
                                        "Y", "Z","¸Û","Ñ§","Ê¹","¾¯","°Ä","¹Ò","¾ü","±±","ÄÏ","¹ã","Éò","À¼","³É","¼Ã","º£","Ãñ","º½","¿Õ"};



    const int SEGMENTATION_FREE_METHOD = 0;
    const int SEGMENTATION_BASED_METHOD = 1;

    class PipelinePR{
        public:
            GeneralRecognizer *generalRecognizer;
            PlateDetection *plateDetection;
            PlateSegmentation *plateSegmentation;
            FineMapping *fineMapping;
            SegmentationFreeRecognizer *segmentationFreeRecognizer;

            PipelinePR(std::string detector_filename,
                       std::string finemapping_prototxt,std::string finemapping_caffemodel,
                       std::string segmentation_prototxt,std::string segmentation_caffemodel,
                       std::string charRecognization_proto,std::string charRecognization_caffemodel,
                       std::string segmentationfree_proto,std::string segmentationfree_caffemodel
                       );
            ~PipelinePR();



            std::vector<std::string> plateRes;
            std::vector<PlateInfo> RunPiplineAsImage(cv::Mat plateImage,int method);







    };


}
#endif //SWIFTPR_PIPLINE_H
