//
// Created by 庾金科 on 23/10/2017.
//

#include "Pipeline.h"


namespace pr {



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
        delete generalRecognizer;
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