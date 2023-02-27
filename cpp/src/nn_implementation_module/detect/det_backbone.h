//
// Created by Tunm-Air13 on 2023/2/8.
//
#pragma once
#ifndef ZEPHYRLPR_DET_BACKBONE_H
#define ZEPHYRLPR_DET_BACKBONE_H

#include "inference_helper_module/inference_helper.h"
#include "plate_det_common.h"
#include "opencv2/opencv.hpp"

namespace hyper {

class DetBackbone {
public:

    DetBackbone();

    ~DetBackbone();

    int32_t Initialize(const std::string& model_filename, cv::Size input_size = cv::Size(320, 320), bool use_half = false);

    int32_t Inference(const cv::Mat &bgr_pad);

public:

    std::vector<OutputTensorInfo> &getMOutputTensorInfoList();

private:

    cv::Size m_input_image_size_{};                             // 输入图像宽高

    std::shared_ptr<InferenceHelper> m_nn_infer_;               // 推理模块

    std::vector<InputTensorInfo> m_input_tensor_info_list_;

    std::vector<OutputTensorInfo> m_output_tensor_info_list_;

};

} // namespace;

#endif //ZEPHYRLPR_DET_BACKBONE_H
