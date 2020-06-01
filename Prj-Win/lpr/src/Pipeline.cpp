#include "../include/Pipeline.h"

namespace pr {
	PipelinePR::PipelinePR(std::string detect_prototxt, std::string detect_caffemodel,
		std::string finetune_prototxt, std::string finetune_caffemodel,
		std::string platerec_prototxt, std::string platerec_caffemodel,
		std::string dbstring)
	{
		platedetection = new PlateDetection(detect_prototxt, detect_caffemodel);
		finetune = new FineTune(finetune_prototxt, finetune_caffemodel);
		platerecognation = new PlateRecognation(platerec_prototxt, platerec_caffemodel);
		dbdetection = new DBDetection(dbstring);
	}
	PipelinePR::~PipelinePR()
	{
		delete platedetection;
		delete finetune;
		delete platerecognation;
		delete dbdetection;
	}
    cv::Mat DBcropFromImage(const cv::Mat &image){
		cv::Mat cropped;
        image.copyTo(cropped);
		int cropped_w = cropped.cols;
		int cropped_h = cropped.rows;
		cv::Rect up,down;
		up.y = cropped_h*0.05;up.x = cropped_w*0.2;up.height = cropped_h*0.35;up.width = cropped_w*0.55;
		down.y = cropped_h*0.4;down.x = cropped_w*0.05;down.height = cropped_h*0.6;down.width = cropped_w*0.95;
		cv::Mat cropUp,cropDown;
		cropped(up).copyTo(cropUp);
		cropped(down).copyTo(cropDown);
		cv::resize(cropUp,cropUp,cv::Size(64,40));
		cv::resize(cropDown,cropDown,cv::Size(96,40));
		cv::Mat crop = cv::Mat(40,160,CV_8UC3);
		cropUp.copyTo(crop(cv::Rect(0,0,64,40)));
		cropDown.copyTo(crop(cv::Rect(64,0,96,40)));
        return crop;

    }
	std::vector<PlateInfo> PipelinePR::RunPiplineAsImage(cv::Mat plateimg,int IsDB)
	{
		std::vector<pr::PlateInfo> plates;
		std::vector<PlateInfo> plateres;
		if(IsDB==1)
		{
			dbdetection->DBDetect(plateimg,plates,30,1280);
		}
		else
		{
			platedetection->Detectssd(plateimg, plates);
		}
		for (pr::PlateInfo plateinfo : plates) {
			cv::Mat image = plateinfo.getPlateImage();
			cv::Mat CropImg;
			
			if(IsDB==1)
			{
				CropImg = DBcropFromImage(image);
				platerecognation->segmentation_free_recognation(CropImg, plateinfo);
			}
			else
			{
				finetune->Finetune(image, CropImg);
				platerecognation->segmentation_free_recognation(CropImg, plateinfo);
			}
			plateres.push_back(plateinfo);
		}
		return plateres;
	}
}
