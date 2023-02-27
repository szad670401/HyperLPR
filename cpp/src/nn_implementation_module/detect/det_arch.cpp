//
// Created by tunm on 2023/2/8.
//

#include "det_arch.h"
#include "log.h"

namespace hyper {


DetArch::DetArch() = default;


int32_t DetArch::Initialize(const std::string& backbone_path, const std::string& head_path, int input_size, int threads,
                            float box_conf_threshold, float nms_threshold, bool use_half) {
    int32_t ret;
    m_input_size_ = input_size;
    m_box_conf_threshold_ = box_conf_threshold;
    m_nms_threshold_ = nms_threshold;
    m_backbone_net_ = std::make_shared<DetBackbone>();
    m_header_net_ = std::make_shared<DetHeader>();
    ret = m_backbone_net_->Initialize(backbone_path, cv::Size(input_size, input_size), use_half);
    if (ret != InferenceHelper::kRetOk) {
        LOGE("Backbone model error.");
        return InferenceHelper::kRetErr;
    }
    ret = m_header_net_->Initialize(head_path, input_size, use_half);
    if (ret != InferenceHelper::kRetOk) {
        LOGE("Head model error.");
        return InferenceHelper::kRetErr;
    }

    if (input_size == 320) {
        m_grid_num_ = 6300;
    } else if (input_size == 640) {
        m_grid_num_ = 25200;
    } else {
        return InferenceHelper::kRetErr;
    }

    return InferenceHelper::kRetOk;
}

template<class T>
void DetArch::Decode(T features, std::vector<PlateLocation> &outputs, float scale, float conf_threshold,
                     float nms_threshold) {

    int item_num = 15;
    for (int index = 0; index < m_grid_num_; ++index) {
        auto det_score = features[index * item_num + 4];
        if (det_score > conf_threshold) {
//            std::cout << det_score << std::endl;
            PlateLocation plate;
            plate.det_confidence = det_score;
            auto class_score_1 = features[index * item_num + 13] * det_score;
            auto class_score_2 = features[index * item_num + 14] * det_score;
            LayersNum type = class_score_1 > class_score_2 ? MONO : DOUBLE;
            plate.layers = type;
            auto x = features[index * item_num + 0];
            auto y = features[index * item_num + 1];
            auto w = features[index * item_num + 2];
            auto h = features[index * item_num + 3];
            std::vector<float> xyxy = xywh2xyxy(x, y, w, h);
            plate.x1 = xyxy[0];
            plate.y1 = xyxy[1];
            plate.x2 = xyxy[2];
            plate.y2 = xyxy[3];
            for (int i = 0; i < 8; ++i) {
                plate.kps[i] = features[index * item_num + i + 5];
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

void DetArch::Detection(const cv::Mat &bgr, bool is_resize, float scale) {
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
        cv::copyMakeBorder(resized_img, pad, 0, hpad, 0, wpad, cv::BORDER_CONSTANT, cv::Scalar(127.5, 127.5, 127.5));
//        cv::imwrite("i.jpg", pad);
//        cv::imshow("pad_border", pad);
//        cv::waitKey(0);
    } else {
        pad = bgr;
    }
    double time;

    time = (double)cv::getTickCount();
    m_backbone_net_->Inference(pad);
    time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();

    time = (double)cv::getTickCount();
    m_header_net_->Inference(m_backbone_net_->getMOutputTensorInfoList()[0].GetDataAsFloat(),
                             m_backbone_net_->getMOutputTensorInfoList()[1].GetDataAsFloat(),
                             m_backbone_net_->getMOutputTensorInfoList()[2].GetDataAsFloat());
    time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();

//    std::vector<float> tensor(m_header_net_->getMOutputTensorInfoList()[0].GetDataAsFloat(),
//                              m_header_net_->getMOutputTensorInfoList()[0].GetDataAsFloat() +
//                              m_header_net_->getMOutputTensorInfoList()[0].GetElementNum());



    std::vector<PlateLocation>().swap(m_results_);
    Decode(m_header_net_->getMOutputTensorInfoList()[0].GetDataAsFloat(), m_results_, scale, m_box_conf_threshold_, m_nms_threshold_);


}


} // namespace