//
// Created by Tunm-Air13 on 2023/2/8.
//

#ifndef ZEPHYRLPR_DET_HEADER_H
#define ZEPHYRLPR_DET_HEADER_H

#include "inference_helper_module/inference_helper.h"
#include "plate_det_common.h"
#include "opencv2/opencv.hpp"

namespace hyper {

typedef struct {
    std::string input_name;
    std::vector<int> dims;
} Header;

typedef std::vector<Header> HeaderList;

class DetHeader {
public:
    DetHeader(const DetHeader &) = delete;

    DetHeader &operator=(const DetHeader &) = delete;

    explicit DetHeader();

    ~DetHeader();

    int32_t Initialize(const std::string& model_filename, int input_size = 320, bool use_half = false);

    int32_t Inference(float* ptr3, float* ptr2, float* ptr1);

    std::vector<OutputTensorInfo> &getMOutputTensorInfoList();

    void setMOutputTensorInfoList(const std::vector<OutputTensorInfo> &mOutputTensorInfoList);

private:

    HeaderList m_header_list_;

    std::unique_ptr<InferenceHelper> m_nn_infer_;               // 推理模块

    std::vector<InputTensorInfo> m_input_tensor_info_list_;

    std::vector<OutputTensorInfo> m_output_tensor_info_list_;

};

} // namespace

#endif //ZEPHYRLPR_DET_HEADER_H
