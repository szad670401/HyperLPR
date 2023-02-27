//
// Created by Tunm-Air13 on 2022/12/30.
//

#include "plate_recognition.h"
#include <utility>
#include "utils.h"
#include "plate_recognition_tokenize.h"

namespace hyper {

PlateRecognition::PlateRecognition() = default;

int PlateRecognition::Initialize(const std::string& model_filename, cv::Size input_size, int threads, float confidence_threshold,
                                 bool use_half) {
    m_input_image_size_ = std::move(input_size);
    m_confidence_threshold_ = confidence_threshold;
    float mean[] = {127.5f, 127.5f, 127.5f};
    float normal[] = {1.0 / 127.5f, 1.0 / 127.5f, 1.0 / 127.5f};
    m_nn_adapter_ = std::make_shared<MNNAdapterInference>(model_filename, threads, mean, normal, use_half);
    m_nn_adapter_->Initialization("data", "output", m_input_image_size_.width, m_input_image_size_.height);

    return 0;
}

TextLine PlateRecognition::Inference(const cv::Mat &bgr_pad) {
    std::vector<float> output = m_nn_adapter_->Invoking(bgr_pad);
//    SLOG_INFO("out: {}", output.size());
    auto line = decode(output);
    return line;
}

TextLine PlateRecognition::decode(const std::vector<float>& tensor) {
    int text_total_num = 24;
    int classify_num = 75;
    IndexList index_list;
    std::vector<float> max_list;
    for (int index = 0; index < text_total_num; ++index) {
        std::vector<float> mapping;
        mapping.reserve(classify_num);
        for (int i = 0; i < classify_num; ++i) {
            mapping.push_back(tensor[index * classify_num + i]);
        }
        auto max_index = argmax(mapping.begin(), mapping.end());
        float max_value = mapping[max_index];
        index_list.push_back(max_index);
        max_list.push_back(max_value);
    }

    TextLine line;
    line.code = "";
    float total_ = 0.0f;
    for (int i = 0; i < index_list.size(); ++i) {
        auto &idx = index_list[i];
        IndexList::const_iterator result = std::find(m_ignored_tokens_.begin(), m_ignored_tokens_.end(), idx);
        if (result == m_ignored_tokens_.end()) {
            if ((i > 0) && index_list[i - 1] == idx) {
                continue;   // remove_duplicate
            }
//            std::cout << "b " << idx << std::endl;
            line.char_index.push_back(idx);
            line.char_scores.push_back(max_list[i]);
            line.code += TOKENIZE[idx];
//            SLOG_INFO("{}", TOKENIZE[idx]);
            total_ += max_list[i];
        }
    }
    line.average_score = total_ / line.char_scores.size();

    return line;
}

const cv::Size &PlateRecognition::getMInputImageSize() const {
    return m_input_image_size_;
}

float PlateRecognition::getMConfidenceThreshold() const {
    return m_confidence_threshold_;
}


}