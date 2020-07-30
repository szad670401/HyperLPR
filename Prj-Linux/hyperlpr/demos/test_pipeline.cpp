//
// Created by Jack Yu on 23/10/2017.
//

#include "../include/Pipeline.h"




using namespace std;

template<class T>
static unsigned int levenshtein_distance(const T &s1, const T &s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++) prevCol[i] = i;
    for (unsigned int i = 0; i < len1; i++) {
        col[0] = i + 1;
        for (unsigned int j = 0; j < len2; j++)
            col[j + 1] = min(
                    min(prevCol[1 + j] + 1, col[j] + 1),
                    prevCol[j] + (s1[i] == s2[j] ? 0 : 1));
        col.swap(prevCol);
    }
    return prevCol[len2];
}




void TEST_ACC(){

    pr::PipelinePR prc("model/cascade.xml",
                       "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                       "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                       "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
                       "model/SegmenationFree-Inception.prototxt","model/SegmenationFree-Inception.caffemodel"
    );

    ifstream file;
    string imagename;
    int n = 0,correct = 0,j = 0,sum = 0;
    char filename[] = "/Users/yujinke/Downloads/general_test/1.txt";
    string pathh = "/Users/yujinke/Downloads/general_test/";
    file.open(filename, ios::in);
    while (!file.eof())
    {
        file >> imagename;
        string imgpath = pathh + imagename;
        std::cout << "------------------------------------------------" << endl;
        cout << "图片名：" << imagename << endl;
        cv::Mat image = cv::imread(imgpath);
//		cv::imshow("image", image);
//		cv::waitKey(0);

        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);

        float conf = 0;
        vector<float> con ;
        vector<string> name;
        for (auto st : res) {
            if (st.confidence > 0.1) {
                //std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                con.push_back(st.confidence);
                name.push_back(st.getPlateName());
                //conf += st.confidence;
            }
            else
                cout << "no string" << endl;
        }
        //	std::cout << conf << std::endl;
        int num = con.size();
        float max = 0;
        string platestr, chpr, ch;
        int diff = 0,dif = 0;
        for (int i = 0; i < num; i++) {

            if (con.at(i) > max)
            {
                max = con.at(i);
                platestr = name.at(i);
            }

        }
        //	cout << "max:"<<max << endl;
        cout << "string:" << platestr << endl;
        chpr = platestr.substr(0, 2);
        ch = imagename.substr(0, 2);
        diff = levenshtein_distance(imagename, platestr);
        dif = diff - 4;
        cout << "差距:" <<dif << endl;
        sum += dif;
        if (ch != chpr) n++;
        if (diff == 0)	correct++;
        j++;
    }
    float cha = 1 - float(n) / float(j);
    std::cout << "------------------------------------------------" << endl;
    cout << "车牌总数：" << j << endl;
    cout << "汉字识别准确率："<<cha << endl;
    float chaccuracy = 1 - float(sum - n * 2) /float(j * 8);
    cout << "字符识别准确率：" << chaccuracy << endl;

}


void TEST_PIPELINE(){

    pr::PipelinePR prc("model/cascade.xml",
                      "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                      "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                      "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
                       "model/SegmenationFree-Inception.prototxt","model/SegmenationFree-Inception.caffemodel"
                    );

    cv::Mat image = cv::imread("/Users/yujinke/ClionProjects/cpp_ocr_demo/test.png");


    std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);

    for(auto st:res) {
        if(st.confidence>0.75) {
            std::cout << st.getPlateName() << " " << st.confidence << std::endl;
            cv::Rect region = st.getPlateRect();

            cv::rectangle(image,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
        }
    }

    cv::imshow("image",image);
    cv::waitKey(0);

}


void TEST_CAM()
{

    cv::VideoCapture capture("test1.mp4");
    cv::Mat frame;

    pr::PipelinePR prc("model/cascade.xml",
                       "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                       "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                       "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel",
                       "model/SegmentationFree.prototxt","model/SegmentationFree.caffemodel"
    );





    while(1) {
        //读取下一帧
        if (!capture.read(frame)) {
            std::cout << "读取视频失败" << std::endl;
            exit(1);
        }
//
//        cv::transpose(frame,frame);
//        cv::flip(frame,frame,2);

//        cv::resize(frame,frame,cv::Size(frame.cols/2,frame.rows/2));



        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(frame,pr::SEGMENTATION_FREE_METHOD);

        for(auto st:res) {
            if(st.confidence>0.75) {
                std::cout << st.getPlateName() << " " << st.confidence << std::endl;
                cv::Rect region = st.getPlateRect();

                cv::rectangle(frame,cv::Point(region.x,region.y),cv::Point(region.x+region.width,region.y+region.height),cv::Scalar(255,255,0),2);
            }
        }

        cv::imshow("image",frame);
        cv::waitKey(1);



    }

}


int main()
{
    TEST_ACC();

//    TEST_CAM();
//    TEST_PIPELINE();

    return 0 ;


}
