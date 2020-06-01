#ifndef _PLATERECOGNATION_H_
#define _PLATERECOGNATION_H_
#include <opencv2/dnn.hpp>
#include "PlateInfo.h"
namespace pr {
	class PlateRecognation {
	public:
		PlateRecognation(std::string Rec_prototxt, std::string Rec_cafffemodel);
		void segmentation_free_recognation(cv::Mat src, pr::PlateInfo &plateinfo);
	private:
		cv::dnn::Net RecNet;
	};
}//namespace pr
#endif // !_PLATERECOGNATION_H_

