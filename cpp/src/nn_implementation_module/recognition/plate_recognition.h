//
// Created by Tunm-Air13 on 2022/12/30.
//
#pragma once
#ifndef ZEPHYRLPR_PLATE_RECOGNITION_H
#define ZEPHYRLPR_PLATE_RECOGNITION_H

#include "nn_module/mnn_adapter.h"
#include "configuration.h"
#include "recognition_commom.h"
#include "basic_types.h"

namespace hyper {

class PlateRecognition {
public:

    PlateRecognition(const PlateRecognition &) = delete;

    PlateRecognition &operator=(const PlateRecognition &) = delete;

    explicit PlateRecognition();

    int Initialize(const std::string& model_filename, cv::Size input_size = REC_INPUT_SIZE, int threads = 1,
                   float confidence_threshold = 0.5, bool use_half = false);

    TextLine Inference(const cv::Mat &bgr_pad);

    const cv::Size &getMInputImageSize() const;

    float getMConfidenceThreshold() const;

private:

    TextLine decode(const std::vector<float>& tensor);

private:

    cv::Size m_input_image_size_{};                            // 输入图像宽高

    float m_confidence_threshold_{};                             // 置信度阈值

    std::shared_ptr<MNNAdapterInference> m_nn_adapter_;        // 推理模块

    IndexList m_ignored_tokens_ = {0, };                       // 需要被忽略的索引

};

}

#endif //ZEPHYRLPR_PLATE_RECOGNITION_H
