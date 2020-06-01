#include "../include/Platedetect.h"

using namespace cv;
using namespace std;
namespace pr {

	PlateDetection::PlateDetection(std::string ssd_prototxt, std::string ssd_caffemodel)
	{
		ssdNet = cv::dnn::readNetFromCaffe(ssd_prototxt, ssd_caffemodel);
	}
	DBDetection::DBDetection(std::string cascadestring)
	{
		dbcascade.load(cascadestring);
	}
	void PlateDetection::Detectssd(cv::Mat img, std::vector<pr::PlateInfo>  &plateInfos)
	{
		int cols = img.cols;
		int rows = img.rows;
		Mat in;
		img.convertTo(in, CV_32F);
		Mat input(img.size(), CV_32FC3);
		Mat inputblob1 = input.reshape(1, { 1, 3,rows,cols });
		Mat input_blob = dnn::blobFromImages(in, 0.225, Size(), Scalar(103.53, 116.28, 123.675), false);
		float *blobdata = input_blob.ptr<float>();
		float *blobdata2 = inputblob1.ptr<float>();
		{
			for (int i = 0; i < rows; i++)
			{
				memcpy(blobdata2 + i * cols, blobdata + 3 * i * cols, cols * sizeof(float));
				memcpy(blobdata2 + i * cols + rows * cols, blobdata + (1 + 3 * i) * cols, cols * sizeof(float));
				memcpy(blobdata2 + i * cols + rows * cols * 2, blobdata + (2 + 3 * i) * cols, cols * sizeof(float));
			}
		}
		ssdNet.setInput(inputblob1);

		Mat outputBlob = ssdNet.forward("detection_out");

		Mat detectmat(outputBlob.size[2], outputBlob.size[3], CV_32F, outputBlob.ptr<float>());
		vector<Rect> recs;
		vector<float>scs;
		for (int i = 0; i < detectmat.rows; i++)
		{
			float confidence = detectmat.at<float>(i, 2);
			if (confidence > 0.5)
			{
				int x1, x2, y1, y2;
				Rect rec;
				Mat cimg;
				x1 = int(detectmat.at<float>(i, 3) * cols);
				y1 = int(detectmat.at<float>(i, 4) * rows);
				x2 = int(detectmat.at<float>(i, 5) * cols);
				y2 = int(detectmat.at<float>(i, 6) * rows);
				x1 = max(x1, 0);
				y1 = max(y1, 0);
				x2 = min(x2, cols - 1);
				y2 = min(y2, rows - 1);
				rec.x = x1; rec.y = y1; rec.width = (x2 - x1 + 1); rec.height = (y2 - y1 + 1);
				img(rec).copyTo(cimg);
				PlateInfo plateInfo(cimg, rec);
				plateInfos.push_back(plateInfo);
			}
		}
	}
    cv::Mat cropFromImage(const cv::Mat &image,cv::Rect rect){
        int w = image.cols-1;
        int h = image.rows-1;
        rect.x = std::max(rect.x,0);
        rect.y = std::max(rect.y,0);
        rect.height = std::min(rect.height,h-rect.y);
        rect.width = std::min(rect.width,w-rect.x);
        cv::Mat temp(rect.size(), image.type());
        cv::Mat cropped;
        temp = image(rect);
        temp.copyTo(cropped);
        return cropped;
    }
	void DBDetection::DBDetect(cv::Mat InputImage,std::vector<pr::PlateInfo> &plateInfos,int min_w,int max_w)
	{
		cv::Mat processImage;
        cv::cvtColor(InputImage,processImage,cv::COLOR_BGR2GRAY);
        std::vector<cv::Rect> platesRegions;
        cv::Size minSize(min_w,min_w/4);
        cv::Size maxSize(max_w,max_w/4);
        if (&processImage == NULL)
            return;
        dbcascade.detectMultiScale( processImage, platesRegions,
                                  1.1, 3, cv::CASCADE_SCALE_IMAGE,minSize,maxSize);
        for(auto plate:platesRegions)
        {
            int zeroadd_w  = static_cast<int>(plate.width*0.28);
            int zeroadd_h = static_cast<int>(plate.height*0.35);
            int zeroadd_x = static_cast<int>(plate.width*0.14);
            int zeroadd_y = static_cast<int>(plate.height*0.15);
            plate.x-=zeroadd_x;
            plate.y-=zeroadd_y;
            plate.height += zeroadd_h;
            plate.width += zeroadd_w;
			cv::Mat plateImage = cropFromImage(InputImage,plate);
            PlateInfo plateInfo(plateImage,plate);
            plateInfos.push_back(plateInfo);
        }
    }
}
