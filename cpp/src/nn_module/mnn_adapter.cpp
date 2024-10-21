//
// Created by tunm on 2022/4/24.
//

#include "mnn_adapter.h"

namespace hyper {


MNNAdapterInference::MNNAdapterInference(const std::string &model, int thread, const float *mean,
                                         const float *normal, bool is_use_half,
                                         bool use_model_bin, bool is_use_cuda) {

    if (is_use_cuda) {
        backend_ = MNN_FORWARD_CUDA;
    } else {
        backend_ = MNN_FORWARD_CPU;
    }
    if (use_model_bin) {
        raw_model_ = std::shared_ptr<MNN::Interpreter>(
                MNN::Interpreter::createFromBuffer(model.c_str(), model.size()));
    } else {
        raw_model_ = std::shared_ptr<MNN::Interpreter>(
                MNN::Interpreter::createFromFile(model.c_str()));
    }

    _config.type = backend_;
    //_config.layers = MNN_FORWARD_CUDA;
    _config.numThread = 2;
    //_config.numThread = thread;
    MNN::BackendConfig backendConfig;
    if (is_use_half) {
        backendConfig.precision = MNN::BackendConfig::Precision_Low;
    } else {
        backendConfig.precision = MNN::BackendConfig::Precision_High;
    }
    backendConfig.power = MNN::BackendConfig::Power_High;
    _config.backendConfig = &backendConfig;
    for (int i = 0; i < 3; i++) {
        this->mean[i] = mean[i];
        this->normal[i] = normal[i];
    }
}


MNNAdapterInference::~MNNAdapterInference() {
    raw_model_->releaseModel();
    raw_model_->releaseSession(sess);
}

void
MNNAdapterInference::Initialization(const std::string &input, const std::string &output, int width, int height) {
    sess = raw_model_->createSession(_config);
    tensor_shape_.resize(4);
    tensor_shape_ = {1, 3, height, width};
    input_ = raw_model_->getSessionInput(sess, input.c_str());
    output_ = raw_model_->getSessionOutput(sess, output.c_str());
    width_ = width;
    height_ = height;
}

std::vector<float> MNNAdapterInference::Invoking(const cv::Mat &mat) {
    assert(mat.rows == height_);
    assert(mat.cols == width_);
    MNN::CV::ImageProcess::Config config;
    config.destFormat = image_format_dst_;
    config.sourceFormat = image_format_src_;
    for (int i = 0; i < 3; i++) {
        config.mean[i] = mean[i];
        config.normal[i] = normal[i];
    }


    std::unique_ptr<MNN::CV::ImageProcess> process(
            MNN::CV::ImageProcess::create(config));
    process->convert(mat.data, mat.cols, mat.rows, (int) mat.step1(), input_);
    raw_model_->runSession(sess);
    auto dimType = input_->getDimensionType();

    if (output_->getType().code != halide_type_float) {
        dimType = MNN::Tensor::TENSORFLOW;
    }
    std::shared_ptr<MNN::Tensor> outputUser(new MNN::Tensor(output_, dimType));
    output_->copyToHostTensor(outputUser.get());
    auto type = outputUser->getType();
    auto size = outputUser->elementSize();
    std::vector<float> tempValues(size);
    if (type.code == halide_type_float) {
        auto values = outputUser->host<float>();
        for (int i = 0; i < size; ++i) {
            tempValues[i] = values[i];
        }
    }
    return tempValues;
}

void MNNAdapterInference::setImageFormatSrc(MNN::CV::ImageFormat imageFormatSrc) {
    image_format_src_ = imageFormatSrc;
}

void MNNAdapterInference::setImageFormatDst(MNN::CV::ImageFormat imageFormatDst) {
    image_format_dst_ = imageFormatDst;
}


}

