//
// Created by Tunm-Air13 on 2022/4/24.
//

#include "camera_buffer.h"

namespace hyper {

CameraBuffer::CameraBuffer() {
    config_.sourceFormat = MNN::CV::YUV_NV21;
    config_.destFormat = MNN::CV::BGR;
    config_.filterType = MNN::CV::NEAREST;
    config_.wrap = MNN::CV::ZERO;
    rotation_mode_ = ROTATION_0;
}

void CameraBuffer::SetDataBuffer(const uint8_t *data_buffer, int height, int width) {
    this->buffer_ = data_buffer;
    this->height_ = height;
    this->width_ = width;
}

void CameraBuffer::SetRotationMode(ROTATION_MODE mode) {
    rotation_mode_ = mode;
}

void CameraBuffer::SetDataFormat(DATA_FORMAT data_format) {
    if (data_format == NV21) {
        config_.sourceFormat = MNN::CV::YUV_NV21;
    }
    if (data_format == NV12) {
        config_.sourceFormat = MNN::CV::YUV_NV12;
    }
    if (data_format == RGBA) {
        config_.sourceFormat = MNN::CV::RGBA;
    }
    if (data_format == RGB) {
        config_.sourceFormat = MNN::CV::RGB;
    }
    if (data_format == BGR) {
        config_.sourceFormat = MNN::CV::BGR;
    }
    if (data_format == BGRA) {
        config_.sourceFormat = MNN::CV::BGRA;
    }
    if (data_format == YCrCb) {
        config_.sourceFormat = MNN::CV::YCrCb;
    }
}

cv::Mat CameraBuffer::GetAffineRGBImage(const cv::Mat &affine_matrix, const int width_out, const int height_out) const {
    int sw = width_;
    int sh = height_;
    int rot_sw = sw;
    int rot_sh = sh;
    MNN::CV::Matrix tr;
    assert(affine_matrix.rows == 2);
    assert(affine_matrix.cols == 3);
    assert(affine_matrix.type() == CV_64F);
    cv::Mat trans_matrix;
    affine_matrix.convertTo(trans_matrix, CV_32F);
    std::vector<float> tr_cv({1, 0, 0, 0, 1, 0, 0, 0, 1});
    memcpy(tr_cv.data(), trans_matrix.data, sizeof(float) * 6);
    tr.set9(tr_cv.data());
    MNN::CV::Matrix tr_inv;
    tr.invert(&tr_inv);
    std::shared_ptr<MNN::CV::ImageProcess> process(
            MNN::CV::ImageProcess::create(config_));
    process->setMatrix(tr_inv);
    cv::Mat img_out(height_out, width_out, CV_8UC3);
    std::shared_ptr<MNN::Tensor> tensor(MNN::Tensor::create<uint8_t>(
            std::vector<int>{1, height_out, width_out, 3}, img_out.data));
    process->convert(buffer_, sw, sh, 0, tensor.get());
    return img_out;
}

cv::Mat CameraBuffer::GetScaledImage(const float scale, bool with_rotation) {
    int sw = width_;
    int sh = height_;
    int rot_sw = sw;
    int rot_sh = sh;
    // MNN::CV::Matrix tr;
    std::shared_ptr<MNN::CV::ImageProcess> process(
            MNN::CV::ImageProcess::create(config_));
    if (rotation_mode_ == ROTATION_270 && with_rotation) {
        float srcPoints[] = {
                0.0f,
                0.0f,
                0.0f,
                (float)(height_ - 1),
                (float)(width_ - 1),
                0.0f,
                (float)(width_ - 1),
                (float)(height_ - 1),
        };
        float dstPoints[] = {(float)(height_ * scale - 1),
                             0.0f,
                             0.0f,
                             0.0f,
                             (float)(height_ * scale - 1),
                             (float)(width_ * scale - 1),
                             0.0f,
                             (float)(width_ * scale - 1)};

        tr_.setPolyToPoly((MNN::CV::Point *)dstPoints,
                          (MNN::CV::Point *)srcPoints, 4);
        process->setMatrix(tr_);
        int scaled_height = static_cast<int>(width_ * scale);
        int scaled_width = static_cast<int>(height_ * scale);
        cv::Mat img_out(scaled_height, scaled_width, CV_8UC3);
        std::shared_ptr<MNN::Tensor> tensor(MNN::Tensor::create<uint8_t>(
                std::vector<int>{1, scaled_height, scaled_width, 3}, img_out.data));
        process->convert(buffer_, sw, sh, 0, tensor.get());
        return img_out;
    } else if (rotation_mode_ == ROTATION_90 && with_rotation) {
        float srcPoints[] = {
                0.0f,
                0.0f,
                0.0f,
                (float)(height_ - 1),
                (float)(width_ - 1),
                0.0f,
                (float)(width_ - 1),
                (float)(height_ - 1),
        };
        float dstPoints[] = {
                0.0f,
                (float)(width_ * scale - 1),
                (float)(height_ * scale - 1),
                (float)(width_ * scale - 1),
                0.0f,
                0.0f,
                (float)(height_ * scale - 1),
                0.0f,
        };
        tr_.setPolyToPoly((MNN::CV::Point *)dstPoints,
                          (MNN::CV::Point *)srcPoints, 4);
        process->setMatrix(tr_);
        int scaled_height = static_cast<int>(width_ * scale);
        int scaled_width = static_cast<int>(height_ * scale);
        cv::Mat img_out(scaled_height, scaled_width, CV_8UC3);
        std::shared_ptr<MNN::Tensor> tensor(MNN::Tensor::create<uint8_t>(
                std::vector<int>{1, scaled_height, scaled_width, 3}, img_out.data));
        process->convert(buffer_, sw, sh, 0, tensor.get());
        return img_out;
    } else if (rotation_mode_ == ROTATION_180 && with_rotation) {
        float srcPoints[] = {
                0.0f,
                0.0f,
                0.0f,
                (float)(height_ - 1),
                (float)(width_ - 1),
                0.0f,
                (float)(width_ - 1),
                (float)(height_ - 1),
        };
        float dstPoints[] = {
                (float)(width_ * scale - 1),
                (float)(height_ * scale - 1),
                (float)(width_ * scale - 1),
                0.0f,
                0.0f,
                (float)(height_ * scale - 1),
                0.0f,
                0.0f,
        };
        tr_.setPolyToPoly((MNN::CV::Point *)dstPoints,
                          (MNN::CV::Point *)srcPoints, 4);
        process->setMatrix(tr_);
        int scaled_height = static_cast<int>(height_ * scale);
        int scaled_width = static_cast<int>(width_ * scale);
        cv::Mat img_out(scaled_height, scaled_width, CV_8UC3);
        std::shared_ptr<MNN::Tensor> tensor(MNN::Tensor::create<uint8_t>(
                std::vector<int>{1, scaled_height, scaled_width, 3}, img_out.data));
        process->convert(buffer_, sw, sh, 0, tensor.get());
        return img_out;
    } else {
        float srcPoints[] = {
                0.0f,
                0.0f,
                0.0f,
                (float)(height_ - 1),
                (float)(width_ - 1),
                0.0f,
                (float)(width_ - 1),
                (float)(height_ - 1),
        };
        float dstPoints[] = {
                0.0f,
                0.0f,
                0.0f,
                (float)(height_ * scale - 1),
                (float)(width_ * scale - 1),
                0.0f,
                (float)(width_ * scale - 1),
                (float)(height_ * scale - 1),
        };
        tr_.setPolyToPoly((MNN::CV::Point *)dstPoints,
                          (MNN::CV::Point *)srcPoints, 4);
        process->setMatrix(tr_);
        int scaled_height = static_cast<int>(height_ * scale);
        int scaled_width = static_cast<int>(width_ * scale);
        cv::Mat img_out(scaled_height, scaled_width, CV_8UC3);
        std::shared_ptr<MNN::Tensor> tensor(MNN::Tensor::create<uint8_t>(
                std::vector<int>{1, scaled_height, scaled_width, 3}, img_out.data));
        process->convert(buffer_, sw, sh, 0, tensor.get());
        return img_out;
    }
}

cv::Mat CameraBuffer::GetAffineMatrix() const {
    cv::Mat affine_matrix(3, 3, CV_32F);
    tr_.get9((float *)affine_matrix.data);
    cv::Mat affine = affine_matrix.rowRange(0, 2);
    cv::Mat affine_64;
    affine.convertTo(affine_64, CV_64F);
    assert(affine_64.rows == 2);
    assert(affine_64.cols == 3);
    assert(affine_64.type() == CV_64F);
    return affine_64;
}

int CameraBuffer::GetHeight() const {
    return height_;
}

int CameraBuffer::GetWidth() const {
    return width_;
}


int CameraBuffer::GetRotationMode() const{
    return rotation_mode_;
}

}