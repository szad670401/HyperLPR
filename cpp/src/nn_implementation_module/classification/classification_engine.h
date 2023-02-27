//
// Created by Tunm-Air13 on 2023/2/9.
//
#pragma once
#ifndef ZEPHYRLPR_CLASSIFICATION_ENGINE_H
#define ZEPHYRLPR_CLASSIFICATION_ENGINE_H

#include "inference_helper_module/inference_helper.h"
#include "configuration.h"
#include "plate_cls_common.h"
#include "basic_types.h"
#include "opencv2/opencv.hpp"

namespace hyper {

class ClassificationEngine {
public:
    ClassificationEngine(const ClassificationEngine &) = delete;

    ClassificationEngine &operator=(const ClassificationEngine &) = delete;

    explicit ClassificationEngine();

    ~ClassificationEngine();

    int32_t Initialize(const std::string& model_filename, cv::Size input_size = CLS_INPUT_SIZE, int threads = 1, bool use_half = false);

    int32_t Inference(const cv::Mat &bgr_pad);

    PlateColor getMOutputColor() const;

    const cv::Size &getMInputImageSize() const;

    float getMOutputMaxConfidence() const;

private:

    cv::Size m_input_image_size_{};                            // 输入图像宽高

    std::unique_ptr<InferenceHelper> m_nn_infer_;               // 推理模块

    std::vector<InputTensorInfo> m_input_tensor_info_list_;

    std::vector<OutputTensorInfo> m_output_tensor_info_list_;

    PlateColor m_output_color_;

    float m_output_max_confidence_;

};

} // namespace

#endif //ZEPHYRLPR_CLASSIFICATION_ENGINE_H
