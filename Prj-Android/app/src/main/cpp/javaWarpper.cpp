#include <jni.h>
#include <string>
#include "include/Pipeline.h"

#include <android/log.h>
#include <android/bitmap.h>

#include <opencv2/opencv.hpp>

using namespace cv;
#define LOG_TAG "System.out"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
jobject mat_to_bitmap(JNIEnv * env, Mat & src, bool needPremultiplyAlpha, jobject bitmap_config){

    jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetStaticMethodID(java_bitmap_class,
                                           "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class,
                                                 mid, src.size().width, src.size().height, bitmap_config);
    AndroidBitmapInfo  info;
    void* pixels = 0;

    try {
        //validate
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        CV_Assert(src.type() == CV_8UC1 || src.type() == CV_8UC3 || src.type() == CV_8UC4);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);

        //type mat
        if(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888){
            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if(src.type() == CV_8UC1){
                cvtColor(src, tmp, CV_GRAY2RGBA);
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, CV_RGB2RGBA);
            } else if(src.type() == CV_8UC4){
                if(needPremultiplyAlpha){
                    cvtColor(src, tmp, COLOR_RGBA2mRGBA);
                }else{
                    src.copyTo(tmp);
                }
            }

        } else{
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if(src.type() == CV_8UC1){
                cvtColor(src, tmp, CV_GRAY2BGR565);
            } else if(src.type() == CV_8UC3){
                cvtColor(src, tmp, CV_RGB2BGR565);
            } else if(src.type() == CV_8UC4){
                cvtColor(src, tmp, CV_RGBA2BGR565);
            }
        }
        AndroidBitmap_unlockPixels(env, bitmap);
        return bitmap;
    } catch(cv::Exception e){
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());
        return bitmap;
    } catch (...){
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, "Unknown exception in JNI code {nMatToBitmap}");
        return bitmap;
    }
}

std::string jstring2str(JNIEnv* env, jstring jstr)
{
    char*   rtn   =   NULL;
    jclass   clsstring   =   env->FindClass("java/lang/String");
    jstring   strencode   =   env->NewStringUTF("GB2312");
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize   alen   =   env->GetArrayLength(barr);
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
    if(alen   >   0)
    {
        rtn   =   (char*)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return   stemp;
}





extern "C" {
JNIEXPORT jlong JNICALL
Java_pr_platerecognization_PlateRecognition_InitPlateRecognizer(
        JNIEnv *env, jobject obj,
        jstring detector_filename,
        jstring finemapping_prototxt, jstring finemapping_caffemodel,
        jstring segmentation_prototxt, jstring segmentation_caffemodel,
        jstring charRecognization_proto, jstring charRecognization_caffemodel,
        jstring segmentationfree_proto, jstring segmentationfree_caffemodel) {

    std::string detector_path = jstring2str(env, detector_filename);
    std::string finemapping_prototxt_path = jstring2str(env, finemapping_prototxt);
    std::string finemapping_caffemodel_path = jstring2str(env, finemapping_caffemodel);
    std::string segmentation_prototxt_path = jstring2str(env, segmentation_prototxt);
    std::string segmentation_caffemodel_path = jstring2str(env, segmentation_caffemodel);
    std::string charRecognization_proto_path = jstring2str(env, charRecognization_proto);
    std::string charRecognization_caffemodel_path = jstring2str(env, charRecognization_caffemodel);
    std::string segmentationfree_proto_path = jstring2str(env, segmentationfree_proto);
    std::string segmentationfree_caffemodel_path = jstring2str(env, segmentationfree_caffemodel);


    pr::PipelinePR *PR = new pr::PipelinePR(detector_path,
                                            finemapping_prototxt_path, finemapping_caffemodel_path,
                                            segmentation_prototxt_path, segmentation_caffemodel_path,
                                            charRecognization_proto_path, charRecognization_caffemodel_path,
                                            segmentationfree_proto_path, segmentationfree_caffemodel_path);

    return (jlong) PR;
}


JNIEXPORT jstring JNICALL
Java_pr_platerecognization_PlateRecognition_SimpleRecognization(
        JNIEnv *env, jobject obj,
        jlong matPtr, jlong object_pr) {
    pr::PipelinePR *PR = (pr::PipelinePR *) object_pr;
    cv::Mat &mRgb = *(cv::Mat *) matPtr;
    cv::Mat rgb;
    cv::cvtColor(mRgb,rgb,cv::COLOR_RGBA2BGR);


    //1表示SEGMENTATION_BASED_METHOD在方法里有说明
    std::vector<pr::PlateInfo> list_res= PR->RunPiplineAsImage(rgb,pr::SEGMENTATION_FREE_METHOD);
//    std::vector<pr::PlateInfo> list_res= PR->RunPiplineAsImage(rgb,1);
    std::string concat_results;
    for(auto one:list_res)
    {
        //可信度
        if (one.confidence>0.7)
            concat_results+=one.getPlateName()+",";
    }

    concat_results = concat_results.substr(0,concat_results.size()-1);

    return env->NewStringUTF(concat_results.c_str());

}

/**
 * 车牌号的详细信息
 * @param env
 * @param obj
 * @param matPtr
 * @param object_pr
 * @return
 */
JNIEXPORT jobject JNICALL
Java_pr_platerecognization_PlateRecognition_PlateInfoRecognization(
        JNIEnv *env, jobject obj,
        jlong matPtr, jlong object_pr) {
    jclass plateInfo_class = env -> FindClass("pr/platerecognization/PlateInfo");
    jmethodID mid = env->GetMethodID(plateInfo_class,"<init>","()V");
    jobject plateInfoObj   = env->NewObject(plateInfo_class,mid);

    pr::PipelinePR *PR = (pr::PipelinePR *) object_pr;
    cv::Mat &mRgb = *(cv::Mat *) matPtr;
    cv::Mat rgb;
    cv::cvtColor(mRgb,rgb,cv::COLOR_RGBA2BGR);

    //1表示SEGMENTATION_BASED_METHOD在方法里有说明
    std::vector<pr::PlateInfo> list_res= PR->RunPiplineAsImage(rgb,pr::SEGMENTATION_FREE_METHOD);
    std::string concat_results;
    pr::PlateInfo plateInfo;
    for(auto one:list_res)
    {
        //可信度
        if (one.confidence>0.7) {
            plateInfo = one;
            //车牌号
            jfieldID fid_plate_name  = env->GetFieldID(plateInfo_class,"plateName","Ljava/lang/String;");
            env->SetObjectField(plateInfoObj,fid_plate_name,env->NewStringUTF(plateInfo.getPlateName().c_str()));

            //识别区域
            Mat src = plateInfo.getPlateImage();

            jclass java_bitmap_class = (jclass)env->FindClass("android/graphics/Bitmap$Config");
            jmethodID bitmap_mid = env->GetStaticMethodID(java_bitmap_class,
                                                          "nativeToConfig", "(I)Landroid/graphics/Bitmap$Config;");
            jobject bitmap_config = env->CallStaticObjectMethod(java_bitmap_class, bitmap_mid, 5);

            jfieldID fid_bitmap = env->GetFieldID(plateInfo_class, "bitmap","Landroid/graphics/Bitmap;");
            jobject _bitmap = mat_to_bitmap(env, src, false, bitmap_config);
            env->SetObjectField(plateInfoObj,fid_bitmap, _bitmap);
            return plateInfoObj;
        }
    }
    return plateInfoObj;

}


JNIEXPORT void JNICALL
Java_pr_platerecognization_PlateRecognition_ReleasePlateRecognizer(
        JNIEnv *env, jobject obj,
        jlong object_re) {
//    std::string hello = "Hello from C++";
    pr::PipelinePR *PR = (pr::PipelinePR *) object_re;
    delete PR;

}
}



