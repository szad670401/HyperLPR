//
// Created by tunm on 2022/4/24.
//

#ifndef ZEPHYRLPR_MNN_ADAPTER_H
#define ZEPHYRLPR_MNN_ADAPTER_H

#include <opencv2/opencv.hpp>
#include <MNN/ImageProcess.hpp>
#include <MNN/Interpreter.hpp>
#include <MNN/MNNDefine.h>
#include <MNN/Tensor.hpp>
#include <MNN/MNNForwardType.h>

namespace hyper {

class MNNAdapterInference {
public:

    MNNAdapterInference(const std::string &model, int thread, const float mean[], const float normal[], bool is_use_half = false,
                        bool use_model_bin = false, bool is_use_cuda = false);

    ~MNNAdapterInference();


    void Initialization(const std::string &input, const std::string &output, int width, int height);


    std::vector<float> Invoking(const cv::Mat &mat);

    void setImageFormatSrc(MNN::CV::ImageFormat imageFormatSrc);

    void setImageFormatDst(MNN::CV::ImageFormat imageFormatDst);

private:
    float mean[3]{};
    float normal[3]{};
    std::shared_ptr<MNN::Interpreter> raw_model_;
    MNN::Tensor *input_{};
    MNN::Tensor *output_{};
    MNN::Session *sess{};
    std::vector<int> tensor_shape_;
    MNN::ScheduleConfig _config;
    MNNForwardType backend_;
    int width_{};
    int height_{};

    MNN::CV::ImageFormat image_format_src_{MNN::CV::ImageFormat::BGR};
    MNN::CV::ImageFormat image_format_dst_{MNN::CV::ImageFormat::BGR};

};

}
#endif //ZEPHYRLPR_MNN_ADAPTER_H
