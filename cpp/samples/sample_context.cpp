//
// Created by tunm on 2023/1/22.
//
#include <iostream>
#include "opencv2/opencv.hpp"
#include "context_module/all.h"
#include "buffer_module/all.h"

using namespace hyper;

static const std::vector<std::string> TYPES = {"蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层"};

int main(int argc, char **argv) {
    char *model_path = argv[1];
    char *image_path = argv[2];
    cv::Mat image = cv::imread(image_path);

    HyperLPRContext context;
    auto ret = context.Initialize(model_path, 5, DetectLevel::DETECT_LEVEL_LOW);
    if (ret != hRetOk) {
        LOGE("Load error.");
        return -1;
    }
    CameraBuffer buffer;
    buffer.SetDataBuffer(image.data, image.rows, image.cols);
    buffer.SetDataFormat(BGR);
    buffer.SetRotationMode(ROTATION_0);
    double time;
    time = (double)cv::getTickCount();
    context(buffer);
    time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();
    LOGD("pipeline cost: %f", time);
    auto &objs = context.getMObjectResults();
    for (auto &obj: objs) {
        cv::rectangle(image,
                      cv::Point2f(obj.x1, obj.y1),
                      cv::Point2f(obj.x2, obj.y2),
                      cv::Scalar(0, 0, 200),
                      2);
        LOGD("[%s]%s", TYPES[obj.type].c_str(), obj.code);
        LOGD("文本均值置信度: %f", obj.text_confidence);
    }

    cv::imshow("w", image);
    cv::waitKey(0);
    return 0;
}