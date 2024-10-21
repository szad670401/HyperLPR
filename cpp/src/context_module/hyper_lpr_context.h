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
    hRetOk = InferenceHelper::kRetOk,           ///< Succeed
    hRetErr = InferenceHelper::kRetErr,         ///< Failure
};


class HyperLPRContext {
public:

    HyperLPRContext(const HyperLPRContext &) = delete;

    HyperLPRContext &operator=(const HyperLPRContext &) = delete;

    explicit HyperLPRContext();

    void operator()(CameraBuffer &buffer);

    void Detect(CameraBuffer &buffer);

    /**
     * Manually initialize and instantiate internal model objects
     * @param models_folder_path Path to the folder containing model files
     * @param max_num Maximum number of license plates to recognize
     * @param detect_level Detector level: 'low' for fast speed, 'high' for slightly higher detection rate but slower speed
     * @param threads Number of inference threads (temporarily ineffective)
     * @param use_half Whether to enable half-precision inference (temporarily ineffective)
     * @param box_conf_threshold Confidence threshold for detection boxes
     * @param nms_threshold Non-maximum suppression threshold
     * @param rec_confidence_threshold Confidence threshold for license plate character recognition
     * @return Initialization status
     */
    int32_t Initialize(const std::string& models_folder_path, 
                    int max_num = 1, 
                    DetectLevel detect_level = DETECT_LEVEL_LOW, 
                    int threads = 1, 
                    bool use_half = false, 
                    float box_conf_threshold = 0.3f,
                    float nms_threshold = 0.5f,
                    float rec_confidence_threshold = 0.75f);

    PlateResultList &getMObjectResults();

    static PlateType PreGetPlateType(std::string& code);

    int32_t getMInitStatus() const;

    PlateResultList &getMDetectResults();

private:
    std::shared_ptr<PlateDetector> m_plate_detector_;

    std::shared_ptr<ClassificationEngine> m_plate_classification_;

    std::shared_ptr<RecognitionEngine> m_plate_recognition_;

    int m_pre_image_size_ = 320;

    PlateResultList m_object_results_;

    PlateResultList m_detect_results_;

    int m_rec_max_num_ = 1;

    int32_t m_init_status_ = hRetErr;

};

} // namespace



#endif //ZEPHYRLPR_HYPER_LPR_CONTEXT_H
