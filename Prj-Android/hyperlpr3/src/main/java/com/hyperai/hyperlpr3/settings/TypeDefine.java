package com.hyperai.hyperlpr3.settings;

public class TypeDefine {

    /** 四种情况的转角 */
    public static final int CAMERA_ROTATION_0 = 0;
    public static final int CAMERA_ROTATION_90 = 1;
    public static final int CAMERA_ROTATION_180 = 2;
    public static final int CAMERA_ROTATION_270 = 3;

    /** 低开销检测模式 */
    public static final int DETECT_LEVEL_LOW = 0;
    /** 高开销检测模式 */
    public static final int DETECT_LEVEL_HIGH = 1;

    /** Image in RGB format - RGB排列格式的图像 */
    public static final int STREAM_RGB = 0;
    /** Image in BGR format (Opencv Mat default) - BGR排列格式的图像(OpenCV的Mat默认) */
    public static final int STREAM_BGR = 1;
    /** Image in RGB with alpha channel format -带alpha通道的RGB排列格式的图像 */
    public static final int STREAM_RGBA = 2;
    /** Image in BGR with alpha channel format -带alpha通道的BGR排列格式的图像 */
    public static final int STREAM_BGRA = 3;
    /** Image in YUV NV12 format - YUV NV12排列的图像格式 */
    public static final int STREAM_YUV_NV12 = 4;
    /** Image in YUV NV21 format - YUV NV21排列的图像格式 */
    public static final int STREAM_YUV_NV21 = 5;


    /** 未知车牌 */
    public static final int PLATE_TYPE_UNKNOWN = -1;
    /** 蓝牌 */
    public static final int PLATE_TYPE_BLUE = 0;
    /** 黄牌单层 */
    public static final int PLATE_TYPE_YELLOW_SINGLE = 1;
    /** 白牌单层 */
    public static final int PLATE_TYPE_WHILE_SINGLE = 2;
    /** 绿牌新能源 */
    public static final int PLATE_TYPE_GREEN = 3;
    /** 黑牌港澳 */
    public static final int PLATE_TYPE_BLACK_HK_MACAO = 4;
    /** 香港单层 */
    public static final int PLATE_TYPE_HK_SINGLE = 5;
    /** 香港双层 */
    public static final int PLATE_TYPE_HK_DOUBLE = 6;
    /** 澳门单层 */
    public static final int PLATE_TYPE_MACAO_SINGLE = 7;
    /** 澳门双层 */
    public static final int PLATE_TYPE_MACAO_DOUBLE = 8;
    /** 黄牌双层 */
    public static final int PLATE_TYPE_YELLOW_DOUBLE = 9;

    public static final String[] PLATE_TYPE_MAPS = {"蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层"};
}
