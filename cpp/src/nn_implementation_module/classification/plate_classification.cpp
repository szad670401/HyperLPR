//
// Created by tunm on 2023/1/22.
//

#include "plate_classification.h"
#include "utils.h"

namespace hyper {

PlateClassification::PlateClassification() = default;

int PlateClassification::Initialize(const std::string& model_filename, cv::Size input_size, int threads, bool use_half) {
    m_input_image_size_ = std::move(input_size);
    float mean[] = {0.0f, 0.0f, 0.0f};
    float normal[] = {0.003921568627f, 0.003921568627f, 0.003921568627f};
    m_nn_adapter_ = std::make_shared<MNNAdapterInference>(model_filename, threads, mean, normal, use_half);
    m_nn_adapter_->Initialization("data", "output", m_input_image_size_.width, m_input_image_size_.height);
    return 0;
}

PlateColor PlateClassification::Inference(const cv::Mat &bgr_pad) {
    std::vector<float> output = m_nn_adapter_->Invoking(bgr_pad);
//    for (int i = 0; i < output.size(); ++i) {
//        std::cout << output[i] << std::endl;
//    }
    auto index = argmax(output.begin(), output.end());
    return PlateColor(index);
}

const cv::Size &PlateClassification::getMInputImageSize() const {
    return m_input_image_size_;
}

} // namespace