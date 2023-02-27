//
// Created by Tunm-Air13 on 2023/2/8.
//

#include "det_header.h"
#include "log.h"

namespace hyper {

#define INPUTS_NUM 3
#define OUTPUT "output"

DetHeader::DetHeader() = default;

DetHeader::~DetHeader() {
    if (!m_nn_infer_) {
        LOGD("Inference helper is not created\n");
        return;
    }
    m_nn_infer_->Finalize();
    m_nn_infer_.reset();
}

int32_t DetHeader::Initialize(const std::string& model_filename, int input_size, bool use_half) {

    m_nn_infer_.reset(InferenceHelper::Create(InferenceHelper::kMnn));
    if (m_nn_infer_->SetNumThreads(1) != InferenceHelper::kRetOk) {
        m_nn_infer_.reset();
        return InferenceHelper::kRetErr;
    }
    if (input_size == 320) {
        m_header_list_ = {
            {"948", { 1, 45, 40, 40 }},
            {"1061", { 1, 45, 20, 20 }},
            {"1174", { 1, 45, 10, 10 }},};
    } else {
        m_header_list_ = {
            {"948", { 1, 45, 80, 80 }},
            {"1061", { 1, 45, 40, 40 }},
            {"1174", { 1, 45, 20, 20 }},};
    }
    m_output_tensor_info_list_.clear();
    m_output_tensor_info_list_.emplace_back(OUTPUT, TensorInfo::kTensorTypeFp32);

    if (m_nn_infer_->Initialize(model_filename, m_input_tensor_info_list_, m_output_tensor_info_list_) != InferenceHelper::kRetOk) {
        m_nn_infer_.reset();
        return InferenceHelper::kRetErr;
    }

    m_input_tensor_info_list_.clear();
    /* Set input tensor info */
    for (int i = 0; i < INPUTS_NUM; ++i) {
        auto &head = m_header_list_[i];
        InputTensorInfo input_tensor_info(head.input_name, TensorInfo::kTensorTypeFp32, false);
        input_tensor_info.tensor_dims =  head.dims;
        input_tensor_info.data_type = InputTensorInfo::kTensorTypeFp32;
        input_tensor_info.normalize.mean[0] = 0.0f;
        input_tensor_info.normalize.mean[1] = 0.0f;
        input_tensor_info.normalize.mean[2] = 0.0f;
        input_tensor_info.normalize.norm[0] = 1.0f;
        input_tensor_info.normalize.norm[1] = 1.0f;
        input_tensor_info.normalize.norm[2] = 1.0f;
        m_input_tensor_info_list_.push_back(input_tensor_info);
    }



    return InferenceHelper::kRetOk;
}

int32_t DetHeader::Inference(float* ptr3, float* ptr2, float* ptr1) {

    m_input_tensor_info_list_[0].data_type = InputTensorInfo::kDataTypeBlobNchw;
    m_input_tensor_info_list_[0].data = ptr3;

    m_input_tensor_info_list_[1].data_type = InputTensorInfo::kDataTypeBlobNchw;
    m_input_tensor_info_list_[1].data = ptr2;

    m_input_tensor_info_list_[2].data_type = InputTensorInfo::kDataTypeBlobNchw;
    m_input_tensor_info_list_[2].data = ptr1;

    if (m_nn_infer_->PreProcess(m_input_tensor_info_list_) != InferenceHelper::kRetOk) {
        return InferenceHelper::kRetErr;
    }
    if (m_nn_infer_->Process(m_output_tensor_info_list_) != InferenceHelper::kRetOk) {
        return InferenceHelper::kRetErr;
    }

    return InferenceHelper::kRetOk;
}


std::vector<OutputTensorInfo> &DetHeader::getMOutputTensorInfoList() {
    return m_output_tensor_info_list_;
}

    void DetHeader::setMOutputTensorInfoList(const std::vector<OutputTensorInfo> &mOutputTensorInfoList) {
        m_output_tensor_info_list_ = mOutputTensorInfoList;
    }
} // namespace