//
// Created by Tunm-Air13 on 2023/2/9.
//

#include "classification_engine.h"
#include "utils.h"


namespace hyper {


ClassificationEngine::ClassificationEngine() {

}

ClassificationEngine::~ClassificationEngine() {

}

int32_t ClassificationEngine::Initialize(const std::string& model_filename, cv::Size input_size, int threads, bool use_half) {
    m_input_image_size_ = std::move(input_size);

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

    InputTensorInfo input_tensor_info("data", TensorInfo::kTensorTypeFp32, true);

    input_tensor_info.tensor_dims =  { 1, 3, m_input_image_size_.height, m_input_image_size_.width };
    input_tensor_info.data_type = InputTensorInfo::kDataTypeImage;

    input_tensor_info.normalize.mean[0] = 0.0f;
    input_tensor_info.normalize.mean[1] = 0.0f;
    input_tensor_info.normalize.mean[2] = 0.0f;
    input_tensor_info.normalize.norm[0] = 0.003921568627f;
    input_tensor_info.normalize.norm[1] = 0.003921568627f;
    input_tensor_info.normalize.norm[2] = 0.003921568627f;

    m_input_tensor_info_list_.push_back(input_tensor_info);

    return InferenceHelper::kRetOk;
}

int32_t ClassificationEngine::Inference(const cv::Mat &bgr_pad) {
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


    auto index = argmax(output_score_raw_list.begin(), output_score_raw_list.end());
    m_output_max_confidence_ = output_score_raw_list[index];
    m_output_color_ = PlateColor(index);

    return InferenceHelper::kRetOk;
}


PlateColor ClassificationEngine::getMOutputColor() const {
    return m_output_color_;
}

const cv::Size &ClassificationEngine::getMInputImageSize() const {
    return m_input_image_size_;
}

float ClassificationEngine::getMOutputMaxConfidence() const {
    return m_output_max_confidence_;
}


} // namespace