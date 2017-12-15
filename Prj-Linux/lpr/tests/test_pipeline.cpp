//
// Created by 庾金科 on 23/10/2017.
//

#include "../include/Pipeline.h"



void TEST_PIPELINE(){

    pr::PipelinePR prc("model/cascade.xml",
                      "model/HorizonalFinemapping.prototxt","model/HorizonalFinemapping.caffemodel",
                      "model/Segmentation.prototxt","model/Segmentation.caffemodel",
                      "model/CharacterRecognization.prototxt","model/CharacterRecognization.caffemodel"
                    );

    cv::Mat image = cv::imread("/Users/yujinke/车牌图片/云南车牌/云A1DZ32.jpg");
    cv::imshow("image",image);
    cv::waitKey(0);

    std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image);
    float conf = 0 ;
    for(auto st:res) {
        if(st.confidence>0.1) {
            std::cout << st.getPlateName() << " " << st.confidence << std::endl;
            conf += st.confidence;
        }
    }
    std::cout<<conf<<std::endl;

}
int main()
{

    TEST_PIPELINE();


    return 0 ;


}