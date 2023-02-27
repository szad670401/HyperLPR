//
// Created by tunm on 2023/2/8.
//
#pragma once
#ifndef ZEPHYRLPR_DET_ARCH_H
#define ZEPHYRLPR_DET_ARCH_H

#include "det_backbone.h"
#include "det_header.h"
#include "plate_det_common.h"

namespace hyper {

class DetArch {
public:
    DetArch(const DetArch &) = delete;

    DetArch &operator=(const DetArch &) = delete;

    explicit DetArch();

    int32_t Initialize(const std::string& backbone_path, const std::string& head_path, int input_size = 320, int threads=1, float box_conf_threshold = 0.3f, float nms_threshold = 0.6f, bool use_half = false);

    void Detection(const cv::Mat& bgr, bool is_resize = false, float scale = 1.0f);

public:
    std::vector<PlateLocation> m_results_;                      // 存放检测结果

private:
    template<class T>
    void Decode(T features, std::vector<PlateLocation> &outputs, float scale,
                float conf_threshold = 0.5, float nms_threshold = 0.5);

private:
    int m_input_size_{};

    float m_box_conf_threshold_{};

    float m_nms_threshold_{};

    std::shared_ptr<DetBackbone> m_backbone_net_;

    std::shared_ptr<DetHeader> m_header_net_;

    int m_grid_num_ = 6300;

};


} // namespace



#endif //ZEPHYRLPR_DET_ARCH_H
