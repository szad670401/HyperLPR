//
// Created by Jack Yu on 29/11/2017.
//
#include "../include/SegmentationFreeRecognizer.h"
#include "../include/Pipeline.h"

#include "../include/PlateInfo.h"



std::string decodeResults(cv::Mat code_table,std::vector<std::string> mapping_table)
{
    cv::MatSize mtsize = code_table.size;
    int sequencelength = mtsize[2];
    int labellength = mtsize[1];
    cv::transpose(code_table.reshape(1,1).reshape(1,labellength),code_table);
    std::string name = "";
    std::vector<int> seq(sequencelength);
    for(int i = 0 ; i < sequencelength;  i++) {
        float *fstart = ((float *) (code_table.data) + i * labellength );
        int id = std::max_element(fstart,fstart+labellength) - fstart;
        seq[i] =id;
    }
    for(int i = 0 ; i< sequencelength ; i++)
    {
        if(seq[i]!=labellength-1 && (i==0 || seq[i]!=seq[i-1]))
            name+=mapping_table[seq[i]];
    }
    std::cout<<name;
    return name;
}


int main()
{
    cv::Mat image = cv::imread("res/cache/chars_segment.jpg");
//    cv::transpose(image,image);

//    cv::resize(image,image,cv::Size(160,40));
    cv::imshow("xxx",image);
    cv::waitKey(0);
    pr::SegmentationFreeRecognizer recognizr("model/SegmenationFree-Inception.prototxt","model/ISegmenationFree-Inception.caffemodel");
    std::pair<std::string,float> res = recognizr.SegmentationFreeForSinglePlate(image,pr::CH_PLATE_CODE);
    std::cout<<res.first<<" "
                  <<res.second<<std::endl;


//    decodeResults(plate,pr::CH_PLATE_CODE);
    cv::imshow("image",image);
    cv::waitKey(0);

    return  0;

}
