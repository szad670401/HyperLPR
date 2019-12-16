#include "../include/Finetune.h"
using namespace std;
using namespace cv;

namespace pr {
	FineTune::FineTune(string finetune_prototxt, string finetune_caffemodel) {
		FTNet = dnn::readNetFromCaffe(finetune_prototxt, finetune_caffemodel);
	}
	void FineTune::affine_crop(Mat src, vector<Point> pts, Mat &crop)
	{
		Point2f dst[4] = { Point2f(0,0),Point2f(160,0),Point2f(160,40),Point2f(0,40) };
		Point2f srcpt[4] = { Point2f(pts[0]),Point2f(pts[1]) ,Point2f(pts[2]) ,Point2f(pts[3]) };
		Mat _mat = getPerspectiveTransform(srcpt, dst);
		warpPerspective(src, crop, _mat, Size(160, 40));
	}
	void FineTune::to_refine(Mat src, vector<Point>pts, Mat& crop)
	{
		float scale = 3.f;
		int cx = 64; int cy = 24;
		int cw = 64; int ch = 24;
		int tx1 = cx - cw / 2;
		int ty1 = cy - ch / 2;
		int tx2 = cx + cw / 2;
		int ty2 = cy - ch / 2;
		int tx3 = cx + cw / 2;
		int ty3 = cy + ch / 2;
		int tx4 = cx - cw / 2;
		int ty4 = cy + ch / 2;
		vector<Point2f> dstp(4);
		Point2f dst[4] = { (Point2f(tx1*scale, ty1*scale)) ,(Point2f(tx2*scale, ty2*scale)) ,(Point2f(tx3*scale, ty3*scale)) ,(Point2f(tx4*scale, ty4*scale)) };
		Point2f pt[4] = { Point2f(pts[0]),Point2f(pts[1]) ,Point2f(pts[2]) ,Point2f(pts[3]) };
		//estimater
		Mat _mat = getPerspectiveTransform(pt, dst);
		warpPerspective(src, crop, _mat, Size(120 * scale, 48 * scale));
	}
	void FineTune::Finetune(Mat src, Mat& dst)
	{
		Mat tof;// = src.clone();
		resize(src, tof, Size(120, 48));
		Mat blob = dnn::blobFromImage(tof, 0.0078125, Size(120, 48), Scalar(127.5, 127.5, 127.5), false, false);
		FTNet.setInput(blob);
		Mat outblob = FTNet.forward("conv6-3");

		float *data = outblob.ptr<float>();
		vector<Point> pts(4);
		Mat fineMat(Size(2, 4), CV_32F, data);
		for (int i = 0; i < fineMat.rows; i++)
		{
			pts[i].x = fineMat.at<float>(i, 0)*src.cols;
			pts[i].y = fineMat.at<float>(i, 1)*src.rows;
		}
		Mat crop;
		to_refine(src, pts, crop);
		blob = dnn::blobFromImage(crop, 0.0078128, Size(120, 48), Scalar(127.5, 127.5, 127.5), false, false);
		FTNet.setInput(blob);
		outblob = FTNet.forward("conv6-3");
		data = outblob.ptr<float>();
		Mat fineMat2(Size(2, 4), CV_32F, data);
		for (int i = 0; i < fineMat.rows; i++)
		{
			pts[i].x = fineMat2.at<float>(i, 0)*crop.cols;
			pts[i].y = fineMat2.at<float>(i, 1)*crop.rows;
		}
		affine_crop(crop, pts, crop);
		dst = crop.clone();
	}
}