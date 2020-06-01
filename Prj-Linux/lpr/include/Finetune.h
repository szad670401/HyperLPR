#ifndef _FINETUNE_H_
#define _FINETUNE_H_
#include<vector>
#include<opencv2/dnn.hpp>
#include<opencv2/opencv.hpp>
namespace pr {
	class FineTune {
	public:

		FineTune(std::string finetune_prototxt, std::string finetune_caffemodel);
		void Finetune(cv::Mat img, cv::Mat &resImg);
		void to_refine(cv::Mat img, std::vector<cv::Point> pts, cv::Mat &out);
		void affine_crop(cv::Mat img, std::vector<cv::Point> pts, cv::Mat &out);
	private:
		cv::dnn::Net FTNet;
	};
}//namespace pr
#endif // !_FINETUNE_H_
