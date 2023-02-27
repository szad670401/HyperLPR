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

    return 0;
}

void PlateDetector::Decode(const std::vector<float> &tensor, std::vector<PlateLocation> &outputs, float scale,
                           float conf_threshold, float nms_threshold) {
    int grid_num = 6300;
    int item_num = 15;
    for (int index = 0; index < grid_num; ++index) {
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

    for (auto &plate : outputs) {
        plate.x1 = plate.x1 / scale;
        plate.y1 = plate.y1 / scale;
        plate.x2 = plate.x2 / scale;
        plate.y2 = plate.y2 / scale;
        for (int i = 0; i < 4; ++i) {
            plate.kps[i * 2 + 0] = plate.kps[i * 2 + 0] / scale;
            plate.kps[i * 2 + 1] = plate.kps[i * 2 + 1] / scale;
        }
    }
}

void PlateDetector::Detection(const cv::Mat &bgr, bool is_resize, float scale) {
    cv::Mat pad;
    if (is_resize) {
        int ori_w = bgr.cols;
        int ori_h = bgr.rows;

        int w, h;
        if (ori_w > ori_h) {
            scale = (float) m_input_size_ / ori_w;
            w = m_input_size_;
            h = ori_h * scale;
        } else {
            scale = (float) m_input_size_ / ori_h;
            h = m_input_size_;
            w = ori_w * scale;
        }
        int wpad = m_input_size_ - w;
        int hpad = m_input_size_ - h;
        cv::Mat resized_img;
        cv::resize(bgr, resized_img, cv::Size(w, h));
        cv::copyMakeBorder(resized_img, pad, 0, hpad, 0, wpad, cv::BORDER_CONSTANT, 0.0f);
    } else {
        pad = bgr;
    }
//    cv::imshow("a", pad);
    auto output = m_nn_adapter_->Invoking(pad);
    std::vector<PlateLocation>().swap(m_results_);
    Decode(output, m_results_, scale, m_box_conf_threshold_, m_nms_threshold_);


}



}