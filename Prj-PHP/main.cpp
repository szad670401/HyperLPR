//
// Created by Coleflowers on 20/06/2018.
// Updated by Justid on 02/07/2018.
// 
#include <phpcpp.h>
#include <iostream>

#include "PlateSegmentation.h"
#include "CNNRecognizer.h"
#include "Recognizer.h"
#include "PlateDetection.h"
#include "FastDeskew.h"
#include "FineMapping.h"
#include "Pipeline.h"

/* 关闭原opencv报错输出 */
int handleError( int status, const char* func_name,
            const char* err_msg, const char* file_name,
            int line, void* userdata )
{
    //Do nothing -- will suppress console output
    return 0;   //Return value is not used
}

/**
 * 车牌图片识别
 * @params  imgpath 图片的绝对路径
 * @params  modelpath 识别模型所在文件夹路径
 * @params  confidence 可信度要求，可选参数，默认值0.75
 * @return  车牌号 图片路径的图片不存在的话 返回空值
 */
cv::String scan(std::string imgpath, std::string modelpath, double confidence){
    cv::redirectError(handleError);
    try {
        pr::PipelinePR prc(modelpath+"/cascade.xml",
                        modelpath+"/HorizonalFinemapping.prototxt",modelpath+"/HorizonalFinemapping.caffemodel",
                        modelpath+"/Segmentation.prototxt",modelpath+"/Segmentation.caffemodel",
                        modelpath+"/CharacterRecognization.prototxt",modelpath+"/CharacterRecognization.caffemodel",
                        modelpath+"/SegmenationFree-Inception.prototxt",modelpath+"/SegmenationFree-Inception.caffemodel"
                    );
        cv::Mat image = cv::imread(imgpath);
        std::vector<pr::PlateInfo> res = prc.RunPiplineAsImage(image,pr::SEGMENTATION_FREE_METHOD);

        cv::String platenum = "";
        for(auto st:res) {
            if(st.confidence>confidence) {
                platenum = st.getPlateName();
                break;
            }
        }

        return platenum;
    }
    catch( cv::Exception& e )
    {
        const char* err_msg = e.what();
        throw Php::Exception(err_msg);
    }

}


Php::Value funcWrapper(Php::Parameters &params) {
    // 图片路径
    std::string img = params[0]; 
    // 模型路径（文件夹）
    std::string model = params[1]; 
    // 可信度要求
    double confidence = 0.75;

    if (params.size() == 3){
        confidence = (double)params[2];
    }

    cv::String res = scan(img, model, confidence);
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
        extension.add<funcWrapper>("platescan", {
            Php::ByVal("imgpath", Php::Type::String, true),
            Php::ByVal("modelpath", Php::Type::String, true),
            Php::ByVal("confidence", Php::Type::Float, false)
        });       
 
        // return the extension
        return extension;
    }
}
