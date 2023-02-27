//
// Created by tunm on 2023/1/25.
//

#ifndef ZEPHYRLPR_HYPER_LPR_CONTEXT_H
#define ZEPHYRLPR_HYPER_LPR_CONTEXT_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "nn_implementation_module/all.h"
#include "buffer_module/all.h"
#include "hyper_lpr_common.h"

namespace hyper {

enum {
    hRetOk = InferenceHelper::kRetOk,           ///< 成功
    hRetErr = InferenceHelper::kRetErr,         ///< 失败
};


class HyperLPRContext {
public:

    HyperLPRContext(const HyperLPRContext &) = delete;

    HyperLPRContext &operator=(const HyperLPRContext &) = delete;

    explicit HyperLPRContext();

    void operator()(CameraBuffer &buffer);

    /**
     * 手动初始化并实例化内部模型对象
     * @param models_folder_path 存放模型文件夹的路径地址
     * @param max_num 最大识别车牌数量
     * @param detect_level 检测器等级 low速度快，high速度慢检出率略高于low
     * @param threads 推理线程数量 (暂时无效)
     * @param use_half 是否开启半精度推理 (暂时无效)
     * @param box_conf_threshold 检测框置信度阈值
     * @param nms_threshold 非极大值抑制阈值
     * @param rec_confidence_threshold 车牌字符识别置信度阈值
     * @return 初始化状态
     */
    int32_t Initialize(const std::string& models_folder_path, int max_num = 1, DetectLevel detect_level = DETECT_LEVEL_LOW, int threads = 1, bool use_half = false, float box_conf_threshold = 0.3f,
                     float nms_threshold = 0.5f,
                    float rec_confidence_threshold = 0.75f);

    PlateResultList &getMObjectResults();

    static PlateType PreGetPlateType(std::string& code);

    int32_t getMInitStatus() const;

private:
    std::shared_ptr<DetArch> m_plate_detector_;

    std::shared_ptr<ClassificationEngine> m_plate_classification_;

    std::shared_ptr<RecognitionEngine> m_plate_recognition_;

    int m_pre_image_size_ = 320;

    PlateResultList m_object_results_;

    int m_rec_max_num_ = 1;

    int32_t m_init_status_ = hRetErr;

};

} // namespace



#endif //ZEPHYRLPR_HYPER_LPR_CONTEXT_H
