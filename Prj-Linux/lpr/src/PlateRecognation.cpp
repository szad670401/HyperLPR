#include "../include/PlateRecognation.h"
#include "../include/Pipeline.h"
using namespace std;
using namespace cv;
namespace pr {

	PlateRecognation::PlateRecognation(std::string rec_prototxt, std::string rec_caffemodel)
	{
		RecNet = cv::dnn::readNetFromCaffe(rec_prototxt, rec_caffemodel);
	}
	void PlateRecognation::segmentation_free_recognation(cv::Mat src, pr::PlateInfo &plateinfo)
	{
		float score = 0;
		string text = "";
		Mat src1 = src.clone();
		Mat inputMat(Size(40, 160), CV_8UC3);

		for (int j = 0; j < src.rows; j++)
		{
			for (int i = 0; i < src.cols; i++)
			{
				inputMat.at<Vec3b>(i, j) = src1.at<Vec3b>(j, i);
			}
		}
		Mat blob = dnn::blobFromImage(inputMat, 1 / 255.f, Size(40, 160), Scalar(0, 0, 0), false, false);
		RecNet.setInput(blob);
		Mat outblob = RecNet.forward();
		int x = outblob.size[2];
		int y = outblob.size[0];
		float *data = outblob.ptr<float>();
		vector<float> scores(84);
		vector<int>maxidxs;
		vector<float> maxscore;
		for (int i = 2; i < 20; i++)
		{
			for (int j = 0; j < 84; j++)
			{
				scores[j] = data[j * 20 + i];
			}
			int idx = max_element(scores.begin(), scores.end()) - scores.begin();
			maxidxs.push_back(idx);
			maxscore.push_back(scores[idx]);
		}
		int charnum = 0;
		for (int i = 0; i < maxidxs.size(); i++)
		{
			if (maxidxs[i] < pr::CH_PLATE_CODE.size() && (i == 0 || (maxidxs[i - 1] != maxidxs[i])))
			{
				text += pr::CH_PLATE_CODE[maxidxs[i]];
				score += maxscore[i];
				charnum++;
			}
		}
		if (charnum > 0)
		{
			score /= charnum;
		}
		plateinfo.setPlateName(text);
		plateinfo.confidence = score;
	}
}