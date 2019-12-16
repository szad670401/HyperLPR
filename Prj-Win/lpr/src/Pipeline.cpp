#include "../include/Pipeline.h"

namespace pr {
	PipelinePR::PipelinePR(std::string detect_prototxt, std::string detect_caffemodel,
		std::string finetune_prototxt, std::string finetune_caffemodel,
		std::string platerec_prototxt, std::string platerec_caffemodel/*,
		std::string platecolor_color*/)
	{
		platedetection = new PlateDetection(detect_prototxt, detect_caffemodel);
		finetune = new FineTune(finetune_prototxt, finetune_caffemodel);
		platerecognation = new PlateRecognation(platerec_prototxt, platerec_caffemodel);
		//platecolorclass = new PlateColorClass(platecolor_color);
	}
	PipelinePR::~PipelinePR()
	{
		delete platedetection;
		delete finetune;
		delete platerecognation;
		//delete platecolorclass;
	}
	std::vector<PlateInfo> PipelinePR::RunPiplineAsImage(cv::Mat plateimg)
	{
		std::vector<pr::PlateInfo> plates;
		std::vector<PlateInfo> plateres;

		platedetection->Detectssd(plateimg, plates);
		for (pr::PlateInfo plateinfo : plates) {
			cv::Mat image = plateinfo.getPlateImage();
			cv::Mat CropImg;
			finetune->Finetune(image, CropImg);
			platerecognation->segmentation_free_recognation(CropImg, plateinfo);
			//platecolorclass->plateColor(CropImg, plateinfo);
			plateres.push_back(plateinfo);
		}
		return plateres;
	}
}