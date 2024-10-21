//
// Created by tunm on 2023/2/6.
//

#include "recognition_engine.h"
#include <utility>
#include "utils.h"
#include "plate_recognition_tokenize.h"

#define INPUT_NAME "data"

namespace hyper {


RecognitionEngine::RecognitionEngine() = default;


int32_t RecognitionEngine::Initialize(const std::string& model_filename, cv::Size input_size, int threads, float confidence_threshold,
                                      bool use_half) {

    m_input_image_size_ = input_size;
    m_confidence_threshold_ = confidence_threshold;


    /* Set output tensor info */
    m_output_tensor_info_list_.clear();
    m_output_tensor_info_list_.push_back(OutputTensorInfo("output", TensorInfo::kTensorTypeFp32));


    m_nn_infer_.reset(InferenceHelper::Create(InferenceHelper::kMnn));

    if (m_nn_infer_->SetNumThreads(threads) != InferenceHelper::kRetOk) {
        m_nn_infer_.reset();
        return InferenceHelper::kRetErr;
    }

    if (m_nn_infer_->Initialize(model_filename, m_input_tensor_info_list_, m_output_tensor_info_list_) != InferenceHelper::kRetOk) {
        m_nn_infer_.reset();
        return InferenceHelper::kRetErr;
    }

    m_input_tensor_info_list_.clear();

    InputTensorInfo input_tensor_info(INPUT_NAME, TensorInfo::kTensorTypeFp32, true);

    input_tensor_info.tensor_dims =  { 1, 3, m_input_image_size_.height, m_input_image_size_.width };
    input_tensor_info.data_type = InputTensorInfo::kDataTypeImage;

    input_tensor_info.normalize.mean[0] = 127.5f;
    input_tensor_info.normalize.mean[1] = 127.5f;
    input_tensor_info.normalize.mean[2] = 127.5f;
    input_tensor_info.normalize.norm[0] = 0.007843137255f;
    input_tensor_info.normalize.norm[1] = 0.007843137255f;
    input_tensor_info.normalize.norm[2] = 0.007843137255f;

    m_input_tensor_info_list_.push_back(input_tensor_info);

    return InferenceHelper::kRetOk;
}

int32_t RecognitionEngine::Inference(const cv::Mat &bgr_pad, TextLine &line) {
    InputTensorInfo& input_tensor_info = m_input_tensor_info_list_[0];

    input_tensor_info.data = bgr_pad.data;
    input_tensor_info.data_type = InputTensorInfo::kDataTypeImage;
    input_tensor_info.image_info.width = bgr_pad.cols;
    input_tensor_info.image_info.height = bgr_pad.rows;
    input_tensor_info.image_info.channel = bgr_pad.channels();
    input_tensor_info.image_info.crop_x = 0;
    input_tensor_info.image_info.crop_y = 0;
    input_tensor_info.image_info.crop_width = bgr_pad.cols;
    input_tensor_info.image_info.crop_height = bgr_pad.rows;
    input_tensor_info.image_info.is_bgr = true;
    input_tensor_info.image_info.swap_color = false;

    if (m_nn_infer_->PreProcess(m_input_tensor_info_list_) != InferenceHelper::kRetOk) {
        return InferenceHelper::kRetErr;
    }
    if (m_nn_infer_->Process(m_output_tensor_info_list_) != InferenceHelper::kRetOk) {
        return InferenceHelper::kRetErr;
    }

    std::vector<float> output_score_raw_list(m_output_tensor_info_list_[0].GetDataAsFloat(),
                                             m_output_tensor_info_list_[0].GetDataAsFloat() +
                                             m_output_tensor_info_list_[0].GetElementNum());

    decode(output_score_raw_list, line);


    return InferenceHelper::kRetOk;
}

const cv::Size &RecognitionEngine::getMInputImageSize() const {
    return m_input_image_size_;
}


float RecognitionEngine::getMConfidenceThreshold() const {
    return m_confidence_threshold_;
}


int32_t RecognitionEngine::decode(const std::vector<float> &tensor, TextLine &line) {
    int text_total_num = REC_MAX_CHAR_NUM;
    int classify_num = REC_CHAR_CLASS_NUM;
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

    return InferenceHelper::kRetOk;
}

RecognitionEngine::~RecognitionEngine() {
    if (!m_nn_infer_) {
        LOGD("Inference helper is not created\n");
        return;
    }
    m_nn_infer_->Finalize();
    m_nn_infer_.reset();
}

} // namespace