//
// Created by 庾金科 on 23/10/2017.
//

#include "Pipeline.h"


namespace pr {

    std::string str_code[]={"京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙", "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂", "琼", "川", "贵", "云", "藏", "陕", "甘", "青", "宁", "新", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z","港","学","使","警","澳","挂","军","北","南","广","沈","兰","成","济","海","民","航","空"};
    const std::vector<std::string> CH_PLATE_CODE(str_code, str_code+83);


    const int HorizontalPadding = 4;
    PipelinePR::PipelinePR(std::string detector_filename,
                           std::string finemapping_prototxt, std::string finemapping_caffemodel,
                           std::string segmentation_prototxt, std::string segmentation_caffemodel,
                           std::string charRecognization_proto, std::string charRecognization_caffemodel,
                           std::string segmentationfree_proto,std::string segmentationfree_caffemodel) {
        plateDetection = new PlateDetection(detector_filename);
        fineMapping = new FineMapping(finemapping_prototxt, finemapping_caffemodel);
        plateSegmentation = new PlateSegmentation(segmentation_prototxt, segmentation_caffemodel);
        generalRecognizer = new CNNRecognizer(charRecognization_proto, charRecognization_caffemodel);
        segmentationFreeRecognizer =  new SegmentationFreeRecognizer(segmentationfree_proto,segmentationfree_caffemodel);

    }

    PipelinePR::~PipelinePR() {

        delete plateDetection;
        delete fineMapping;
        delete plateSegmentation;
        delete (CNNRecognizer *)generalRecognizer;
        delete segmentationFreeRecognizer;


    }

    std::vector<PlateInfo> PipelinePR:: RunPiplineAsImage(cv::Mat plateImage,int method) {
        std::vector<PlateInfo> results;
        std::vector<pr::PlateInfo> plates;
        plateDetection->plateDetectionRough(plateImage,plates,36,700);

        for (pr::PlateInfo plateinfo:plates) {

            cv::Mat image_finemapping = plateinfo.getPlateImage();
            image_finemapping = fineMapping->FineMappingVertical(image_finemapping);
            image_finemapping = pr::fastdeskew(image_finemapping, 5);



            //Segmentation-based

            if(method==SEGMENTATION_BASED_METHOD)
            {
                image_finemapping = fineMapping->FineMappingHorizon(image_finemapping, 2, HorizontalPadding);
                cv::resize(image_finemapping, image_finemapping, cv::Size(136+HorizontalPadding, 36));
//            cv::imshow("image_finemapping",image_finemapping);
//            cv::waitKey(0);
                plateinfo.setPlateImage(image_finemapping);
                std::vector<cv::Rect> rects;

                plateSegmentation->segmentPlatePipline(plateinfo, 1, rects);
                plateSegmentation->ExtractRegions(plateinfo, rects);
                cv::copyMakeBorder(image_finemapping, image_finemapping, 0, 0, 0, 20, cv::BORDER_REPLICATE);
                plateinfo.setPlateImage(image_finemapping);
                generalRecognizer->SegmentBasedSequenceRecognition(plateinfo);
                plateinfo.decodePlateNormal(pr::CH_PLATE_CODE);

            }
                //Segmentation-free
            else if(method==SEGMENTATION_FREE_METHOD)
            {

                image_finemapping = fineMapping->FineMappingHorizon(image_finemapping, 4, HorizontalPadding+3);

                cv::resize(image_finemapping, image_finemapping, cv::Size(136+HorizontalPadding, 36));
//                cv::imwrite("./test.png",image_finemapping);
//                cv::imshow("image_finemapping",image_finemapping);
//                cv::waitKey(0);
                plateinfo.setPlateImage(image_finemapping);
//                std::vector<cv::Rect> rects;

                std::pair<std::string,float> res = segmentationFreeRecognizer->SegmentationFreeForSinglePlate(plateinfo.getPlateImage(),pr::CH_PLATE_CODE);
                plateinfo.confidence = res.second;
                plateinfo.setPlateName(res.first);
            }



            results.push_back(plateinfo);
        }

//        for (auto str:results) {
//            std::cout << str << std::endl;
//        }
        return results;

    }//namespace pr



}
