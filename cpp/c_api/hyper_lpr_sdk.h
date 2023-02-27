//
// Created by tunm on 2023/1/25.
//

#ifndef ZEPHYRLPR_HYPER_LPR_SDK_H
#define ZEPHYRLPR_HYPER_LPR_SDK_H

#include <stdint.h>

#if defined(_WIN32)
#ifdef HYPER_BUILD_SHARED_LIB
#define HYPER_CAPI_EXPORT __declspec(dllexport)
#else
#define HYPER_CAPI_EXPORT
#endif
#else
#define HYPER_CAPI_EXPORT __attribute__((visibility("default")))
#endif // _WIN32


#ifdef __cplusplus
extern "C" {
#endif

/**
 * API Result - API调用结果
 * */
typedef int HREESULT;

typedef enum HResultCode{
    Ok = 0,
    Err = -1,
} HResultCode;

/**
 * camera stream format - 支持的相机流格式
 * Contains several common camera stream formats on the market -
 * 包含了几款市面上常见的相机流格式
 */
typedef enum HLPR_ImageFormat {
    STREAM_RGB = 0,             ///< Image in RGB format - RGB排列格式的图像
    STREAM_BGR = 1,             ///< Image in BGR format (Opencv Mat default) - BGR排列格式的图像(OpenCV的Mat默认)
    STREAM_RGBA = 2,            ///< Image in RGB with alpha channel format - 带alpha通道的RGB排列格式的图像
    STREAM_BGRA = 3,            ///< Image in BGR with alpha channel format - 带alpha通道的BGR排列格式的图像
    STREAM_YUV_NV12 = 4,        ///< Image in YUV NV12 format - YUV NV12排列的图像格式
    STREAM_YUV_NV21 = 5,        ///< Image in YUV NV21 format - YUV NV21排列的图像格式
} HLPR_ImageFormat;


/**
 * Camera picture corner mode - 相机画面转角模式
 * To cope with the rotation of some devices, four image rotation modes are provided here -
 * 为应对某些设备的画面自带旋转，这里提供四种图像旋转模式
 */
typedef enum HLPR_Rotation {
    CAMERA_ROTATION_0 = 0,      ///< 0 degree - 0
    CAMERA_ROTATION_90 = 1,     ///< 90 degree - 90
    CAMERA_ROTATION_180 = 2,    ///< 180 degree - 180
    CAMERA_ROTATION_270 = 3,    ///< 270 degree - 270
} HLPR_Rotation;

/**
 * Image Buffer Data struct - 图像数据流结构
 * */
typedef struct HLPR_ImageData {
    uint8_t *data;                      ///< Image data stream - 图像数据流
    int width;                          ///< Width of the image - 宽
    int height;                         ///< Height of the image - 高
    HLPR_ImageFormat format;            ///< Format of the image - 传入需要解析数据流格式
    HLPR_Rotation rotation;             ///< The rotation Angle of the image - 图像的画面旋转角角度
} HLPR_ImageData, *P_HLPR_ImageData;


/**
 * Plate layers - 车牌层数
 * */
typedef enum HLPR_PlateLayers {
    PLATE_LAYERS_MONO = 0,       ///< 单层车牌
    PLATE_LAYERS_DOUBLE,     ///< 双层车牌
} HLPR_Layers;


/**
 * Detector Level - 检测器等级
 * */
typedef enum HLPR_DetectLevel {
    DETECT_LEVEL_LOW = 0,       ///< 高开销检测模式 (推荐)
    DETECT_LEVEL_HIGH,          ///< 低开销检测模式
} HLPR_DetectLevel;

/**
 * PlateType Type - 车牌类型(中国)
 * */
typedef enum HLPR_PlateType {
    PLATE_TYPE_UNKNOWN = -1,                         ///< 未知车牌
    PLATE_TYPE_BLUE = 0,                             ///< 蓝牌
    PLATE_TYPE_YELLOW_SINGLE = 1,                    ///< 黄牌单层
    PLATE_TYPE_WHILE_SINGLE = 2,                     ///< 白牌单层
    PLATE_TYPE_GREEN = 3,                            ///< 绿牌新能源
    PLATE_TYPE_BLACK_HK_MACAO = 4,                   ///< 黑牌港澳
    PLATE_TYPE_HK_SINGLE = 5,                        ///< 香港单层
    PLATE_TYPE_HK_DOUBLE = 6,                        ///< 香港双层
    PLATE_TYPE_MACAO_SINGLE = 7,                     ///< 澳门单层
    PLATE_TYPE_MACAO_DOUBLE = 8,                     ///< 澳门双层
    PLATE_TYPE_YELLOW_DOUBLE = 9,                    ///< 黄牌双层
} HLPR_PlateType;

/**
 * Plate Result - 车牌检测结果
 * */
typedef struct HLPR_PlateResult {
    float x1;                                       ///< 左上角点x坐标
    float y1;                                       ///< 左上角点y坐标
    float x2;                                       ///< 右下角点x坐标
    float y2;                                       ///< 右下角点y坐标
    HLPR_PlateType type;                           ///< 车牌类型
    float text_confidence;                           ///< 置信度
    char code[128];                                ///< 车牌号码字符串
} HLPR_PlateResult, *P_HLPR_PlateResult;

/**
 * Plate Result List - 车牌检测结果列表
 * */
typedef struct HLPR_PlateResultList {
    unsigned long plate_size;                       ///< 车牌数量
    P_HLPR_PlateResult plates;                      ///< 检测车牌结果列表
} HLPR_PlateResultList, *P_HLPR_PlateResultList;

/**
 * HyperLPR Context Instantiating parameters - Context的实例化参数对象
 * */
typedef struct HLPR_ContextConfiguration {
    char *models_path;                              ///< 模型文件地址
    int max_num;                                    ///< 识别最大数量
    int threads;                                    ///< 线程数 (推荐1)
    bool use_half;                                  ///< 是否使用半精度推理模式
    float box_conf_threshold;                        ///< 检测框阈值
    float nms_threshold;                             ///< 非极大值抑制阈值
    float rec_confidence_threshold;                   ///< 识别置信度阈值
    HLPR_DetectLevel det_level;                     ///< 检测器等级(推荐low)
} HLPR_ContextConfiguration, *P_HLPR_ContextConfiguration;

/**
 * Data Buffer - 数据缓冲流
 * */
typedef struct HLPR_DataBuffer HLPR_DataBuffer, *P_HLPR_DataBuffer;

/**
 * The runtime object after HyperLPR is instantiated - 实例化运行时的Context对象
 * */
typedef struct HLPR_Context HLPR_Context, *P_HLPR_Context;

/************************************************************************
* Carry parameters to create a data buffer stream instantiation object.
* 携带创建数据缓冲流实例化对象.
* [out] return: Model instant handle - 返回实例化后的指针句柄
************************************************************************/
HYPER_CAPI_EXPORT extern P_HLPR_DataBuffer HLPR_CreateDataBuffer(
        P_HLPR_ImageData data       // [in] Image Buffer Data struct - 图像数据流结构
);

/************************************************************************
* Create a data buffer stream instantiation object.
* 创建数据缓冲流实例化对象.
* [out] return: Model instant handle - 返回实例化后的指针句柄
************************************************************************/
HYPER_CAPI_EXPORT extern P_HLPR_DataBuffer HLPR_CreateDataBufferEmpty();

/************************************************************************
* Set the DataBuffer rotation mode.
* 设置DataBuffer旋转模式.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_DataBufferSetData(
        P_HLPR_DataBuffer buffer,               // [in] DataBuffer handle - 相机流组件的句柄指针
        const uint8_t *data,                    // [in] Raw data stream - 原始的数据流
        int width,                              // [in] Image width - 图像宽度
        int height                              // [in] Image height - 图像高度
);

/************************************************************************
* Set the DataBuffer data format.
* 设置DataBuffer数据格式.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_DataBufferSetRotationMode(
        P_HLPR_DataBuffer buffer,              // [in] DataBuffer handle - 数据流组件的句柄指针
        HLPR_Rotation mode                     // [in] DataBuffer mode - 数据流组件旋转模式
);

/************************************************************************
* Set the DataBuffer rotation mode.
* 设置DataBuffer旋转模式.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_DataBufferSetStreamFormat(
        P_HLPR_DataBuffer buffer,            // [in] DataBuffer handle - 数据流组件的句柄指针
        HLPR_ImageFormat mode                // [in] DataBuffer data format - 数据流组件数据格式
);

/************************************************************************
* Releases the DataBuffer object that has been instantiated.
* 释放已经被实例化后的模型对象.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_ReleaseDataBuffer(
        P_HLPR_DataBuffer buffer             // [in] DataBuffer handle - 相机流组件的句柄指针
);

/************************************************************************
* Create a data Context instantiation object.
* 创建Context实例化对象.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern P_HLPR_Context HLPR_CreateContext(
        P_HLPR_ContextConfiguration configuration       // [in] Context configuration - 配置表
);

/************************************************************************
* Query the Context instantiation state.
* 查询实例化后的状态.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_ContextQueryStatus(
        P_HLPR_Context ctx                      // [in] Context handle - Context的指针句柄
);

/************************************************************************
* Update Data Buffer Stream.
* 喂入数据流并更新进行车牌识别.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_ContextUpdateStream(
        P_HLPR_Context ctx,                     // [in] Context handle - Context的指针句柄
        P_HLPR_DataBuffer buffer,               // [in] DataBuffer handle - 数据流组件的句柄指针
        P_HLPR_PlateResultList results          // [out] Results List - 返回结果的列表
);

/************************************************************************
* Release Context.
* 释放Context的实例化对象.
* [out] Result Code - 返回结果码
************************************************************************/
HYPER_CAPI_EXPORT extern HREESULT HLPR_ReleaseContext(
        P_HLPR_Context ctx                     // [in] Context handle - Context的指针句柄
);

//HYPER_CAPI_EXPORT extern HREESULT HLPR_DataBufferTest(P_HLPR_DataBuffer buffer, const char *save_path);

#ifdef __cplusplus
}
#endif


#endif //ZEPHYRLPR_HYPER_LPR_SDK_H
