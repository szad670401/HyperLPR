#include"../include/Pipeline.h"

void TEST_PIPELINE()
{
	pr::PipelinePR prc("../lpr\\model\\mininet_ssd_v1.prototxt", "../lpr\\model\\mininet_ssd_v1.caffemodel",
		"../lpr\\model\\refinenet.prototxt", "../lpr\\model\\refinenet.caffemodel",
		"../lpr\\model\\SegmenationFree-Inception.prototxt", "../lpr\\model\\SegmenationFree-Inception.caffemodel",
	"../lpr\\model\\cascade_double.xml");
	cv::Mat img = cv::imread("../lpr\\res\\test.jpg");
	std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(img,0);
	for (auto st : res) {
		if (st.confidence > 0.75) {
			std::cout << st.getPlateName() << " " << st.confidence << std::endl;
			cv::Rect region = st.getPlateRect();

			cv::rectangle(img, cv::Point(region.x, region.y), cv::Point(region.x + region.width, region.y + region.height), cv::Scalar(255, 255, 0), 2);
		}
	}

	//cv::imshow("image", img);
	//cv::waitKey(0);

}
int main()
{
	TEST_PIPELINE();
	return 0;
}
