//
// Created by �׽�� on 22/10/2017.
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
				std::string finemapping_prototxt, std::string finemapping_caffemodel,
				std::string segmentation_prototxt, std::string segmentation_caffemodel,
				std::string charRecognization_proto, std::string charRecognization_caffemodel
			) {
				plateDetection = new PlateDetection(detector_filename);
				fineMapping = new FineMapping(finemapping_prototxt, finemapping_caffemodel);
				plateSegmentation = new PlateSegmentation(segmentation_prototxt, segmentation_caffemodel);
				generalRecognizer = new CNNRecognizer(charRecognization_proto, charRecognization_caffemodel); 
			}

			~PipelinePR() {

				delete plateDetection;
				delete fineMapping;
				delete plateSegmentation;
				delete generalRecognizer;
			}

			std::vector<std::string> chars_code{ "京","沪","津","渝","冀","晋","蒙","辽","吉","黑","苏","浙","皖","闽","赣","鲁","豫","鄂","湘","粤","桂","琼","川","贵","云","藏","陕","甘","青","宁","新","0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","J","K","L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z" };

			std::vector<std::string> plateRes;
			std::vector<PlateInfo> RunPiplineAsImage(cv::Mat plateImage) {
				std::vector<PlateInfo> results;
				std::vector<pr::PlateInfo> plates;
				plateDetection->plateDetectionRough(plateImage, plates);

				for (pr::PlateInfo plateinfo : plates) {

					cv::Mat image_finemapping = plateinfo.getPlateImage();
					image_finemapping = fineMapping->FineMappingVertical(image_finemapping);
					image_finemapping = pr::fastdeskew(image_finemapping, 5);
					image_finemapping = fineMapping->FineMappingHorizon(image_finemapping, 2, 5);
					cv::resize(image_finemapping, image_finemapping, cv::Size(136, 36));
					plateinfo.setPlateImage(image_finemapping);
					std::vector<cv::Rect> rects;
					plateSegmentation->segmentPlatePipline(plateinfo, 1, rects);
					plateSegmentation->ExtractRegions(plateinfo, rects);
					cv::copyMakeBorder(image_finemapping, image_finemapping, 0, 0, 0, 20, cv::BORDER_REPLICATE);

					plateinfo.setPlateImage(image_finemapping);
					generalRecognizer->SegmentBasedSequenceRecognition(plateinfo);
					plateinfo.decodePlateNormal(chars_code);
					results.push_back(plateinfo);
					std::cout << plateinfo.getPlateName() << std::endl;


				}

				//        for (auto str:results) {
				//            std::cout << str << std::endl;
				//        }
				return results;
			}




    };


}
#endif //SWIFTPR_PIPLINE_H
