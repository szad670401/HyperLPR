#pragma warning(disable:4430)
#ifndef _PIPLINE_H
#define _PIPLINE_H
#include <vector>
#include "Finetune.h"
#include "Platedetect.h"
#include "PlateRecognation.h"
//#include "PlateColor.h"
using namespace std;
using namespace cv;
namespace pr
{
	const std::vector<std::string> CH_PLATE_CODE{ "京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙", "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂",
		"琼", "川", "贵", "云", "藏", "陕", "甘", "青", "宁", "新", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
		"B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P", "Q", "R", "S", "T", "U", "V", "W", "X",
		"Y", "Z","港","学","使","警","澳","挂","军","北","南","广","沈","兰","成","济","海","民","航","空" };


	class PipelinePR {

		public:
			PlateDetection *platedetection;
			FineTune *finetune;
			PlateRecognation *platerecognation;
			DBDetection *dbdetection;

			

			PipelinePR(std::string detect_prototxt, std::string detect_caffemodel,
				std::string finetune_prototxt, std::string finetune_caffemodel,
				std::string platerec_prototxt, std::string platerec_caffemodel,
				std::string dbstring);
			~PipelinePR();

			std::vector<std::string> plateRes;
			std::vector<PlateInfo> RunPiplineAsImage(cv::Mat srcImage,int IsDB);

	};
}
#endif // !SWIFTPR_PIPLINE_H