//
// Created by tunm on 2023/1/22.
//
#pragma once
#ifndef ZEPHYRLPR_PLATE_CLASSIFICATION_H
#define ZEPHYRLPR_PLATE_CLASSIFICATION_H

#include "nn_module/mnn_adapter.h"
#include "basic_types.h"
#include "plate_cls_common.h"

namespace hyper {

class PlateClassification {
public:

    PlateClassification(const PlateClassification &) = delete;

    PlateClassification &operator=(const PlateClassification &) = delete;

    explicit PlateClassification();

    int Initialize(const std::string& model_filename, cv::Size input_size = cv::Size(96, 96), int threads = 1, bool use_half = false);

    PlateColor Inference(const cv::Mat &bgr_pad);

    const cv::Size &getMInputImageSize() const;

private:

    cv::Size m_input_image_size_{};

    std::shared_ptr<MNNAdapterInference> m_nn_adapter_;
};

} // namespace


#endif //ZEPHYRLPR_PLATE_CLASSIFICATION_H
