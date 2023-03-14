//
// Created by tunm on 2023/1/25.
//
#pragma once

#include <string>
#include "hyper_lpr_context.h"
#include "configuration.h"
#include "utils.h"
#include "log.h"

namespace hyper {

HyperLPRContext::HyperLPRContext() = default;

void HyperLPRContext::operator()(CameraBuffer &buffer) {
    cv::Mat process_image;
    process_image = buffer.GetScaledImage(1.0f, true);
    m_plate_detector_->Detection(process_image, true, 1.0f);
    PlateResultList().swap(m_object_results_);
    auto &detect_results = m_plate_detector_->m_results_;
//    std::sort(detect_results.begin(), detect_results.end(),
//              [](PlateLocation a, PlateLocation b) { return xyxyArea(a.x1, a.y1, a.x2, a.y2) > xyxyArea(b.x1, b.y1, b.x2, b.y2); });
    //
    auto iteration_num = detect_results.size() > m_rec_max_num_ ? m_rec_max_num_: detect_results.size();
    for (size_t i = 0; i < iteration_num; ++i) {
        auto &loc = detect_results[i];
        PlateResult obj;
        obj.x1 = loc.x1;
        obj.y1 = loc.y1;
        obj.x2 = loc.x2;
        obj.y2 = loc.y2;
        cv::Mat align_image;
        getRotateCropAndAlignPad(process_image, align_image, loc.kps);
//        std::cout << align_image_pad.size << std::endl;
//        cv::imshow("align_image_pad", align_image_pad);
//        cv::imshow("align_image", align_image);
//        cv::waitKey(0);


        TextLine text_line;
        if (loc.layers == LayersNum::DOUBLE) {
            int line = (int )((float )align_image.rows * 0.4f);
            int bottom_h = align_image.rows - line;
            cv::Rect_<int> top_rect(0, 0, align_image.cols, line);
            cv::Rect_<int> bottom_rect(0, line, align_image.cols, bottom_h);
            cv::Mat top_crop = align_image(top_rect);
            cv::Mat bottom_crop = align_image(bottom_rect);

            std::vector<cv::Mat> candidate = {top_crop, bottom_crop};
            text_line.code = "";
            text_line.average_score = 0.0f;
            for (int j = 0; j < candidate.size(); ++j) {
                cv::Mat &align = candidate[j];
                cv::Mat align_pad;
                float wh_ratio = (float) align.cols / align.rows;
                imagePadding(align, align_pad, wh_ratio, m_plate_recognition_->getMInputImageSize());
                TextLine candidate_text;
//                cv::imshow("align_pad", align_pad);
//                cv::waitKey(0);
//                if (j == 0)
//                    cv::imwrite("a.jpg", align_pad);
                m_plate_recognition_->Inference(align_pad, candidate_text);
                text_line.code += candidate_text.code;
                text_line.average_score += candidate_text.average_score;
            }
            text_line.average_score /= candidate.size();
//            cv::imshow("top_crop", top_crop);
//            cv::imshow("bottom_crop", bottom_crop);
//            cv::waitKey(0);
        } else {
            cv::Mat align_image_pad;
            float wh_ratio = (float) align_image.cols / align_image.rows;
            imagePadding(align_image, align_image_pad, wh_ratio, m_plate_recognition_->getMInputImageSize());
            m_plate_recognition_->Inference(align_image_pad, text_line);
        }
        cv::resize(align_image, align_image, m_plate_classification_->getMInputImageSize());
//        SLOG_CRITICAL("cfg: {}", text_line.average_score);
//        SLOG_CRITICAL("code: {}", text_line.code);
        if (text_line.average_score < m_plate_recognition_->getMConfidenceThreshold()) {
            continue;
        }
//        cv::imshow("resize", align_image_pad);
//        cv::imshow("align_image", align_image);
//        cv::waitKey(0);
//        obj.color_classify = PlateColor::GREEN;
//        obj.layers = loc.layers;
//        LOGD("size %d", text_line.code.size());
        if (text_line.code.size() >= 8) {
            obj.text_confidence = text_line.average_score;
            auto type = PreGetPlateType(text_line.code);
            if (type == PlateType::UNKNOWN) {
                m_plate_classification_->Inference(align_image);
                auto color_type = m_plate_classification_->getMOutputColor();
//                obj.color_classify = color_type;
//                obj.layers = loc.layers;
                if (color_type == PlateColor::YELLOW) {
                    // 黄牌
                    if (loc.layers == LayersNum::DOUBLE) {
                        // 双层黄牌
                        type = PlateType::YELLOW_DOUBLE;
                    } else {
                        // 单层黄牌
                        type = PlateType::YELLOW_SINGLE;
                    }
                } else if (color_type == PlateColor::BLUE) {
                    type = PlateType::BLUE;
                } else if (color_type == PlateColor::GREEN) {
                    type = PlateType::GREEN;
                }
            }
            obj.type = type;

            strcpy(obj.code, text_line.code.c_str());
//        obj.code = text_line.code;

//        cv::imshow("align_image_pad", align_image_pad);
//        cv::waitKey(0);
            m_object_results_.push_back(obj);
        }
    }
}

int32_t HyperLPRContext::Initialize(const std::string& models_folder_path, int max_num, DetectLevel detect_level,
                                 int threads, bool use_half, float box_conf_threshold, float nms_threshold, float rec_confidence_threshold) {
    int32_t ret;
    std::string det_backbone_model = models_folder_path + "/" + hyper::DETECT_LOW_BACKBONE_FILENAME;
    if (!exists(det_backbone_model)) {
        LOGE("file: %s does not exist.", hyper::DETECT_LOW_BACKBONE_FILENAME.c_str());
        return hRetErr;
    }
    std::string det_header_model = models_folder_path + "/" + hyper::DETECT_LOW_HEAD_FILENAME;
    if (!exists(det_header_model)) {
        LOGE("file: %s does not exist.", hyper::DETECT_LOW_HEAD_FILENAME.c_str());
        return hRetErr;
    }
    if (detect_level == DETECT_LEVEL_HIGH) {
        det_backbone_model = models_folder_path + "/" + hyper::DETECT_HIGH_BACKBONE_FILENAME;
        if (!exists(det_backbone_model)) {
            LOGE("file: %s does not exist.", hyper::DETECT_HIGH_BACKBONE_FILENAME.c_str());
            return hRetErr;
        }
        det_header_model = models_folder_path + "/" + hyper::DETECT_HIGH_HEAD_FILENAME;
        if (!exists(det_header_model)) {
            LOGE("file: %s does not exist.", hyper::DETECT_HIGH_HEAD_FILENAME.c_str());
            return hRetErr;
        }
        m_pre_image_size_ = 640;
    }
    m_plate_detector_ = std::make_shared<DetArch>();
    ret = m_plate_detector_->Initialize(det_backbone_model, det_header_model, m_pre_image_size_, threads, box_conf_threshold, nms_threshold,
                                  use_half);
    if (ret != hRetOk) {
        LOGE("Detect model loading errors.");
        return hRetErr;
    }
    // init classification
    std::string classification_model = models_folder_path + "/" + hyper::CLS_MODEL_FILENAME;
    if (!exists(classification_model)) {
        LOGE("file: %s does not exist.", hyper::CLS_MODEL_FILENAME.c_str());
        return hRetErr;
    }
    m_plate_classification_ = std::make_shared<ClassificationEngine>();
    ret = m_plate_classification_->Initialize(classification_model, CLS_INPUT_SIZE, threads, use_half);
    if (ret != hRetOk) {
        LOGE("Cls model loading errors.");
        return hRetErr;
    }
    // init recognition
    std::string recognition_model = models_folder_path + "/" + hyper::REC_MODEL_FILENAME;
    if (!exists(recognition_model)) {
        LOGE("file: %s does not exist.", hyper::REC_MODEL_FILENAME.c_str());
        return hRetErr;
    }
    m_plate_recognition_ = std::make_shared<RecognitionEngine>();
    ret = m_plate_recognition_->Initialize(recognition_model, REC_INPUT_SIZE, threads, rec_confidence_threshold,
                                     use_half);
    if (ret != hRetOk) {
        LOGE("Rec model loading errors.");
        return hRetErr;
    }
    m_rec_max_num_ = std::max(1, max_num);      // Can't be less than 1

    m_init_status_ = hRetOk;

    return hRetOk;
}

PlateResultList &HyperLPRContext::getMObjectResults() {
    return m_object_results_;
}

PlateType HyperLPRContext::PreGetPlateType(std::string& code) {
    PlateType type = PlateType::UNKNOWN;
    if (code[0] == 'W' && code[1] == 'J'){
        type = PlateType::WHILE_SINGLE;
    } else if (code.size() == 10) {
        type = PlateType::GREEN;
    } else if (code.find("学") != -1) {
        type = PlateType::BLUE;
    } else if (code.find("港") != -1) {
        type = PlateType::BLACK_HK_MACAO;
    } else if (code.find("澳") != -1) {
        type = PlateType::BLACK_HK_MACAO;
    } else if (code.find("警") != -1) {
        type = PlateType::WHILE_SINGLE;
    } else if (code.find("粤Z") != -1) {
        type = PlateType::BLACK_HK_MACAO;
    }

    return type;
}

int32_t HyperLPRContext::getMInitStatus() const {
    return m_init_status_;
}


}


