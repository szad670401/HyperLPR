//
// Created by tunm on 2023/1/27.
//

#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdlib.h>
#include <android/bitmap.h>
//#include <solexcv/log.h>
#include <opencv2/opencv.hpp>
#include "hyper_lpr_sdk.h"
#include "jni/common/jni_utils.h"
#include "log.h"

using namespace hyper;

extern "C" {

static double cost_ = 0.0;

JNIEXPORT long JNICALL HYPERLPR_API(core_HyperLPRCore_CreateRecognizerContext) (
        JNIEnv *env,
        jobject thiz,
        jobject parameterObj) {
    // get object class
    jclass parameterCls = env->GetObjectClass(parameterObj);
    // get object method id
    jmethodID getModelPathMethodId = env->GetMethodID(parameterCls, "getModelPath", "()Ljava/lang/String;");
    jmethodID getThreadsMethodId = env->GetMethodID(parameterCls, "getThreads", "()I");
    jmethodID isUseHalfMethodId = env->GetMethodID(parameterCls, "isUseHalf", "()Z");
    jmethodID getBoxConfThresholdMethodId = env->GetMethodID(parameterCls, "getBoxConfThreshold", "()F");
    jmethodID getNmsThresholdMethodId = env->GetMethodID(parameterCls, "getNmsThreshold", "()F");
    jmethodID getRecConfidenceThresholdMethodId = env->GetMethodID(parameterCls, "getRecConfidenceThreshold", "()F");
    jmethodID getDetLevelMethodId = env->GetMethodID(parameterCls, "getDetLevel", "()I");
    jmethodID getMaxNumMethodId = env->GetMethodID(parameterCls, "getMaxNum", "()I");
    // get model path
    jstring modelPath = (jstring ) env->CallObjectMethod(parameterObj, getModelPathMethodId, NULL);
    std::string path = jstring2str(env, modelPath);
    // get number of threads
    jint threads = env->CallIntMethod(parameterObj, getThreadsMethodId, NULL);
    // get is use half
    jboolean useHalf = env->CallBooleanMethod(parameterObj, isUseHalfMethodId, NULL);
    // get box conf threshold
    jfloat boxConfThreshold = env->CallFloatMethod(parameterObj, getBoxConfThresholdMethodId, NULL);
    // get nms threshold
    jfloat nmsThreshold = env->CallFloatMethod(parameterObj, getNmsThresholdMethodId, NULL);
    // get rec confidence threshold
    jfloat textThreshold = env->CallFloatMethod(parameterObj, getRecConfidenceThresholdMethodId, NULL);
    // get detect level
    jint detLevel = env->CallIntMethod(parameterObj, getDetLevelMethodId, NULL);
    // get max num level
    jint maxNum = env->CallIntMethod(parameterObj, getMaxNumMethodId, NULL);


    LOGD("input path: %s", path.c_str());
    LOGD("threads: %d", threads);
    LOGD("use half: %d", useHalf);
    LOGD("det conf threshold: %f", boxConfThreshold);
    LOGD("nms threshold: %f", nmsThreshold);
    LOGD("text threshold: %f", textThreshold);
    LOGD("det level: %d", detLevel);

    // create context
    HLPR_ContextConfiguration configuration = {0};
    configuration.models_path = const_cast<char*>(path.c_str());
    configuration.det_level = HLPR_DetectLevel(detLevel);
    configuration.use_half = useHalf;
    configuration.nms_threshold = nmsThreshold;
    configuration.rec_confidence_threshold = textThreshold;
    configuration.box_conf_threshold = boxConfThreshold;
    configuration.threads = threads;
    configuration.max_num = maxNum;

    P_HLPR_Context ctx = HLPR_CreateContext(&configuration);

    env->DeleteLocalRef(parameterCls);

    return (long )ctx;
}

JNIEXPORT int JNICALL HYPERLPR_API(core_HyperLPRCore_ReleaseRecognizerContext) (
        JNIEnv *env,
        jobject thiz,
        jlong handle) {
    P_HLPR_Context ctx = (P_HLPR_Context ) handle;

    return HLPR_ReleaseContext(ctx);

}


JNIEXPORT jobjectArray JNICALL HYPERLPR_API(core_HyperLPRCore_PlateRecognitionFromImage) (
        JNIEnv *env,
        jobject thiz,
        jlong handle,
        jintArray buf,
        jint height,
        jint width,
        jint rotation,
        jint format) {
    P_HLPR_Context ctx = (P_HLPR_Context ) handle;
    jint *pBuf = env->GetIntArrayElements(buf, 0);
    // create ImageData
    HLPR_ImageData data = {0};
    data.data = (uint8_t *)pBuf;
    data.width = width;
    data.height = height;
    data.format = HLPR_ImageFormat(format);
    data.rotation = HLPR_Rotation(rotation);
    // create DataBuffer
    P_HLPR_DataBuffer buffer = HLPR_CreateDataBuffer(&data);

    // exec plate recognition
    HLPR_PlateResultList results = {0};
    cost_ = (double)cv::getTickCount();
    HLPR_ContextUpdateStream(ctx, buffer, &results);
    cost_ = ((double)cv::getTickCount() - cost_) / cv::getTickFrequency();
    LOGD("cost: %f", cost_);

    jobjectArray jPlateArray = nullptr;
    jclass jPlateCls = env->FindClass("com/hyperai/hyperlpr3/bean/Plate");
    // get object method id
    jmethodID plateClsInitId = env->GetMethodID(jPlateCls, "<init>", "()V");
    jmethodID setX1MethodId = env->GetMethodID(jPlateCls, "setX1", "(F)V");
    jmethodID setY1MethodId = env->GetMethodID(jPlateCls, "setY1", "(F)V");
    jmethodID setX2MethodId = env->GetMethodID(jPlateCls, "setX2", "(F)V");
    jmethodID setY2MethodId = env->GetMethodID(jPlateCls, "setY2", "(F)V");
    //    jmethodID setLayersMethodId = env->GetMethodID(jPlateCls, "setLayers", "(I)V");
    jmethodID setTypeMethodId = env->GetMethodID(jPlateCls, "setType", "(I)V");
    jmethodID setConfidenceMethodId = env->GetMethodID(jPlateCls, "setConfidence", "(F)V");
    jmethodID setCodeMethodId = env->GetMethodID(jPlateCls, "setCode", "(Ljava/lang/String;)V");

    int total = results.plate_size;
    jPlateArray = env->NewObjectArray(total, jPlateCls, 0);
    for (int i = 0; i < total; ++i) {
        auto &plate = results.plates[i];
        // set in location
        jobject jPlate = env->NewObject(jPlateCls, plateClsInitId);
        env->CallVoidMethod(jPlate, setX1MethodId, plate.x1);
        env->CallVoidMethod(jPlate, setY1MethodId, plate.y1);
        env->CallVoidMethod(jPlate, setX2MethodId, plate.x2);
        env->CallVoidMethod(jPlate, setY2MethodId, plate.y2);
        // set in types
        //        env->CallVoidMethod(jPlate, setLayersMethodId, plate.layers);
        env->CallVoidMethod(jPlate, setTypeMethodId, plate.type);
        // set in text_confidence
        env->CallVoidMethod(jPlate, setConfidenceMethodId, plate.text_confidence);
        // set in plate code
        //        env->CallObjectMethod(jPlate, setCodeMethodId, stringTojstring(env, plate.code));
        env->CallVoidMethod(jPlate, setCodeMethodId, env->NewStringUTF(plate.code));

        env->SetObjectArrayElement(jPlateArray, i, jPlate);
        env->DeleteLocalRef(jPlate);
    }
    env->DeleteLocalRef(jPlateCls);

    return jPlateArray;

}


JNIEXPORT jobjectArray JNICALL HYPERLPR_API(core_HyperLPRCore_PlateRecognitionFromBuffer) (
        JNIEnv *env,
        jobject thiz,
        jlong handle,
        jbyteArray buf,
        jint height,
        jint width,
        jint rotation,
        jint format) {
    P_HLPR_Context ctx = (P_HLPR_Context ) handle;

    uint8_t *pBuf = (uint8_t *) env->GetByteArrayElements(buf, 0);
    // create ImageData
    HLPR_ImageData data = {0};
    data.data = pBuf;
    data.width = width;
    data.height = height;
    data.format = HLPR_ImageFormat(format);
    data.rotation = HLPR_Rotation(rotation);
    // create DataBuffer
    P_HLPR_DataBuffer buffer = HLPR_CreateDataBuffer(&data);

    // exec plate recognition
    HLPR_PlateResultList results = {0};
    cost_ = (double)cv::getTickCount();
    HLPR_ContextUpdateStream(ctx, buffer, &results);
    cost_ = ((double)cv::getTickCount() - cost_) / cv::getTickFrequency();
    LOGD("cost: %f", cost_);

    jobjectArray jPlateArray = nullptr;
    jclass jPlateCls = env->FindClass("com/hyperai/hyperlpr3/bean/Plate");
    // get object method id
    jmethodID plateClsInitId = env->GetMethodID(jPlateCls, "<init>", "()V");
    jmethodID setX1MethodId = env->GetMethodID(jPlateCls, "setX1", "(F)V");
    jmethodID setY1MethodId = env->GetMethodID(jPlateCls, "setY1", "(F)V");
    jmethodID setX2MethodId = env->GetMethodID(jPlateCls, "setX2", "(F)V");
    jmethodID setY2MethodId = env->GetMethodID(jPlateCls, "setY2", "(F)V");
//    jmethodID setLayersMethodId = env->GetMethodID(jPlateCls, "setLayers", "(I)V");
    jmethodID setTypeMethodId = env->GetMethodID(jPlateCls, "setType", "(I)V");
    jmethodID setConfidenceMethodId = env->GetMethodID(jPlateCls, "setConfidence", "(F)V");
    jmethodID setCodeMethodId = env->GetMethodID(jPlateCls, "setCode", "(Ljava/lang/String;)V");

    int total = results.plate_size;
    jPlateArray = env->NewObjectArray(total, jPlateCls, 0);
    for (int i = 0; i < total; ++i) {
        auto &plate = results.plates[i];
        // set in location
        jobject jPlate = env->NewObject(jPlateCls, plateClsInitId);
        env->CallVoidMethod(jPlate, setX1MethodId, plate.x1);
        env->CallVoidMethod(jPlate, setY1MethodId, plate.y1);
        env->CallVoidMethod(jPlate, setX2MethodId, plate.x2);
        env->CallVoidMethod(jPlate, setY2MethodId, plate.y2);
        // set in types
//        env->CallVoidMethod(jPlate, setLayersMethodId, plate.layers);
        env->CallVoidMethod(jPlate, setTypeMethodId, plate.type);
        // set in text_confidence
        env->CallVoidMethod(jPlate, setConfidenceMethodId, plate.text_confidence);
        // set in plate code
//        env->CallObjectMethod(jPlate, setCodeMethodId, stringTojstring(env, plate.code));
        env->CallVoidMethod(jPlate, setCodeMethodId, env->NewStringUTF(plate.code));

        env->SetObjectArrayElement(jPlateArray, i, jPlate);
        env->DeleteLocalRef(jPlate);
    }
    env->DeleteLocalRef(jPlateCls);

    return jPlateArray;
}

} // extern