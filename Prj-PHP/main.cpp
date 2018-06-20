//
// Created by Coleflowers on 20/06/2018.
// 
#include <phpcpp.h>
#include <iostream>

#include "PlateSegmentation.h"
#include "CNNRecognizer.h"
#include "Recognizer.h"
#include "PlateDetection.h"
#include "FastDeskew.h"
#include "FineMapping.h"


std::vector<std::string> chars{"京","沪","津","渝","冀","晋","蒙","辽","吉","黑","苏","浙","皖","闽","赣","鲁","豫","鄂","湘","粤","桂","琼","川","贵","云","藏","陕","甘","青","宁","新","0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F","G","H","J","K","L","M","N","P","Q","R","S","T","U","V","W","X","Y","Z"};

/**
 * 车牌图片识别
 * @param  imgpath 图片的绝对路径
 * @return  车牌号 图片路径的图片不存在的话 返回空值
 */
cv::String scan(const char *imgpath){
    // 读取图片获取车牌的粗略部分
    cv::Mat image = cv::imread(imgpath); 
    if(!image.data) { 
        printf("No img data!\n");
        return "";
    }
    pr::PlateDetection plateDetection("/usr/platescan/src/lpr/model/cascade.xml");
    std::vector<pr::PlateInfo> plates;
    plateDetection.plateDetectionRough(image,plates);
    // todo 处理发现的每一个车牌信息
    // 目前只处理了一组
    cv::Mat img;
    for(pr::PlateInfo platex:plates) {
        // 暂时不保存了
        //cv::imwrite("res/mmm.png",platex.getPlateImage());
        img = platex.getPlateImage(); 
    }
   
    // 车牌部分 
    cv::Mat image_finemapping = pr::FineMapping::FineMappingVertical(img);
    pr::FineMapping finemapper =  pr::FineMapping("/usr/platescan/src/lpr/model/HorizonalFinemapping.prototxt","/usr/platescan/src/lpr/model/HorizonalFinemapping.caffemodel");
    
    // 
    image_finemapping = pr::fastdeskew(image_finemapping, 5);

    // image_finemapping = finemapper.FineMappingHorizon(image_finemapping,0,-3);
    // Android 代码里这个是 
    // 改了之后部分图片数据精度发生变化 ，比如 字母精度有了，但是汉字又错了
    image_finemapping = finemapper.FineMappingHorizon(image_finemapping,2,5);
    // 暂时不保存了
    //cv::imwrite("res/m222222222.png",image_finemapping);

    // 设置尺寸 识别
    cv::Mat demo = image_finemapping; 
    cv::resize(demo,demo,cv::Size(136,36));
    //cv::imwrite("res/m333333.png",demo);

    cv::Mat respones;
    pr::PlateSegmentation plateSegmentation("/usr/platescan/src/lpr/model/Segmentation.prototxt","/usr/platescan/src/lpr/model/Segmentation.caffemodel");
    pr::PlateInfo plate;
    plate.setPlateImage(demo);
    std::vector<cv::Rect> rects;
    plateSegmentation.segmentPlatePipline(plate,1,rects);
    plateSegmentation.ExtractRegions(plate,rects);

    pr::GeneralRecognizer *recognizer = new pr::CNNRecognizer("/usr/platescan/src/lpr/model/CharacterRecognization.prototxt","/usr/platescan/src/lpr/model/CharacterRecognization.caffemodel");
    recognizer->SegmentBasedSequenceRecognition(plate);
    //std::cout<<plate.decodePlateNormal(chars)<<std::endl;
    plate.decodePlateNormal(chars);

    cv::String txt = plate.getPlateName();
    // printf("res:%s\n", txt.c_str());

    //
    // 写入结果到文件
    // FILE *fp;  
    // fp = fopen(resSaveFile, "wb+");
    // fprintf(fp, "%s", txt.c_str());  
    // fclose(fp);
    //
    
    delete(recognizer);
    return txt;
}

Php::Value  myFunction(Php::Parameters &params) {

   std::string str = params[0]; 
   const char *str_c = str.c_str();

   cv::String res = scan(str_c);
   return res.c_str();
}


/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("platescan", "1.0");
        
        // @todo    add your own functions, classes, namespaces to the extension
        extension.add<myFunction>("platescan", {Php::ByRef("string", Php::Type::String)});       
 
        // return the extension
        return extension;
    }
}
