//
// Created by Tunm-Air13 on 2022/4/24.
//

#ifndef SOLEXCV_CAMERA_BUFFER_H
#define SOLEXCV_CAMERA_BUFFER_H

#include <opencv2/opencv.hpp>
#include <MNN/ImageProcess.hpp>
#include <memory>

namespace hyper {

enum ROTATION_MODE {
    ROTATION_0 = 0,
    ROTATION_90 = 1,
    ROTATION_180 = 2,
    ROTATION_270 = 3
};

enum DATA_FORMAT {
    NV21 = 0, NV12 = 1, RGBA = 2, RGB = 3, BGR = 4, BGRA = 5, YCrCb = 6
};

class CameraBuffer {
public:

    CameraBuffer();


    void SetDataBuffer(const uint8_t *data_buffer, int height, int width);


    void SetRotationMode(ROTATION_MODE mode);


    void SetDataFormat(DATA_FORMAT data_format);


    cv::Mat GetAffineRGBImage(const cv::Mat &affine_matrix, const int width_out,
                              const int height_out) const;


    cv::Mat GetScaledImage(const float scale, bool with_rotation);


    cv::Mat GetAffineMatrix() const;

    int GetHeight() const;

    int GetWidth() const;

    int GetRotationMode() const;

private:
    const uint8_t *buffer_;
    int buffer_size_;
    std::vector<float> rotation_matrix;
    int height_;
    int width_;
    MNN::CV::Matrix tr_;
    ROTATION_MODE rotation_mode_;
    MNN::CV::ImageProcess::Config config_;
    std::shared_ptr <MNN::CV::ImageProcess> process_;
};

}
#endif //SOLEXCV_CAMERA_BUFFER_H
