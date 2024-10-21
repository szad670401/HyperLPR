//
// Created by tunm on 2022/12/29.
//

#include "plate_detector.h"

namespace hyper {

PlateDetector::PlateDetector() {

}

int PlateDetector::Initialize(const char *model_path,
                              int input_size,
                              int threads,
                              float box_conf_threshold,
                              float nms_threshold,
                              bool use_half){
    m_input_size_ = input_size;
    m_box_conf_threshold_ = box_conf_threshold;
    m_nms_threshold_ = nms_threshold;
    float mean[] = {0.0f, 0.0f, 0.0f};
    float normal[] = {0.003921568627f, 0.003921568627f, 0.003921568627f};
    m_nn_adapter_ = std::make_shared<MNNAdapterInference>(model_path, threads, mean, normal, use_half);
    m_nn_adapter_->Initialization("input", "output", m_input_size_, m_input_size_);
    if (input_size == 320) {
        m_grid_ = 6300;
    } else if(input_size == 640) {
        m_grid_ = 25200;
    } else {
        std::cerr << "Unsupported detector input size" << std::endl;
    }

    return 0;
}

void PlateDetector::LetterBox(const cv::Mat &img, cv::Mat &output, float &r, int &left, int &top, cv::Size size) {
    int h = img.rows;
    int w = img.cols;
    r = std::min(static_cast<float>(size.width) / h, static_cast<float>(size.height) / w);
    int new_h = static_cast<int>(h * r);
    int new_w = static_cast<int>(w * r);
    top = (size.height - new_h) / 2;
    left = (size.width - new_w) / 2;
    int bottom = size.height - new_h - top;
    int right = size.width - new_w - left;
    cv::Mat img_resize;
    cv::resize(img, img_resize, cv::Size(new_w, new_h));
    cv::cvtColor(img_resize, img_resize, cv::COLOR_BGR2RGB);
    cv::copyMakeBorder(img_resize, output, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
}

void PlateDetector::Decode(const std::vector<float> &tensor, std::vector<PlateLocation> &outputs,
                           float conf_threshold, float nms_threshold) {
    int item_num = 15;
    for (int index = 0; index < m_grid_; ++index) {
        auto det_score = tensor[index * item_num + 4];
        if (det_score > conf_threshold) {
            PlateLocation plate;
            plate.det_confidence = det_score;
            auto class_score_1 = tensor[index * item_num + 13] * det_score;
            auto class_score_2 = tensor[index * item_num + 14] * det_score;
            LayersNum type = class_score_1 > class_score_2 ? MONO : DOUBLE;
            plate.layers = type;
            auto x = tensor[index * item_num + 0];
            auto y = tensor[index * item_num + 1];
            auto w = tensor[index * item_num + 2];
            auto h = tensor[index * item_num + 3];
            std::vector<float> xyxy = xywh2xyxy(x, y, w, h);
            plate.x1 = xyxy[0];
            plate.y1 = xyxy[1];
            plate.x2 = xyxy[2];
            plate.y2 = xyxy[3];
            for (int i = 0; i < 8; ++i) {
                plate.kps[i] = tensor[index * item_num + i + 5];
            }
            outputs.push_back(plate);
        }

    }
    nms(outputs, nms_threshold);

//    for (auto &plate : outputs) {
//        plate.x1 = plate.x1 / scale;
//        plate.y1 = plate.y1 / scale;
//        plate.x2 = plate.x2 / scale;
//        plate.y2 = plate.y2 / scale;
//        for (int i = 0; i < 4; ++i) {
//            plate.kps[i * 2 + 0] = plate.kps[i * 2 + 0] / scale;
//            plate.kps[i * 2 + 1] = plate.kps[i * 2 + 1] / scale;
//        }
//    }
}

void PlateDetector::RestoreBox(std::vector<PlateLocation> &boxes, float r, int left, int top) {
    for (auto &box : boxes) {
        box.x1 = (box.x1 - left) / r;
        box.y1 = (box.y1 - top) / r;
        box.x2 = (box.x2 - left) / r;
        box.y2 = (box.y2 - top) / r;
        for (int i = 0; i < 4; ++i) {
            box.kps[i * 2 + 0] = (box.kps[i * 2 + 0] - left) / r;
            box.kps[i * 2 + 1] = (box.kps[i * 2 + 1] - top) / r;
        }
    }
}

void PlateDetector::Detection(const cv::Mat &bgr) {
    cv::Mat pad;
    float r;
    int left, top;
    LetterBox(bgr, pad, r, left, top, cv::Size(m_input_size_, m_input_size_));
//    cv::imshow("q", pad);
//    cv::waitKey(0);
//    cv::imshow("a", pad);
    auto output = m_nn_adapter_->Invoking(pad);
    m_results_.clear();
    Decode(output, m_results_, m_box_conf_threshold_, m_nms_threshold_);
    RestoreBox(m_results_, r, left, top);

}



}