//
// Created by tunm on 2022/12/29.
//
#pragma once
#ifndef ZEPHYRLPR_PLATE_DETECTOR_H
#define ZEPHYRLPR_PLATE_DETECTOR_H
#include "nn_module/mnn_adapter.h"
#include "plate_det_common.h"

namespace hyper {

class PlateDetector {
public:

    PlateDetector(const PlateDetector &) = delete;

    PlateDetector &operator=(const PlateDetector &) = delete;

    explicit PlateDetector();

    int Initialize(const char *model_path, int input_size = 320, int threads=1, float box_conf_threshold = 0.3f, float nms_threshold = 0.6, bool use_half = false);

    void Detection(const cv::Mat& bgr);

private:
    void Decode(const std::vector<float> &tensor, std::vector<PlateLocation> &outputs,
                float conf_threshold = 0.5, float nms_threshold = 0.5);

    static void LetterBox(const cv::Mat &img, cv::Mat &output, float &r, int &left, int &top, cv::Size size=cv::Size(640, 640));

    void RestoreBox(std::vector<PlateLocation> &boxes, float r, int left, int top);

public:
    std::vector<PlateLocation> m_results_;

private:

    int m_input_size_{};

    float m_box_conf_threshold_{};

    float m_nms_threshold_{};

    int m_grid_{};

    std::shared_ptr<MNNAdapterInference> m_nn_adapter_;

};

}

#endif //ZEPHYRLPR_PLATE_DETECTOR_H
