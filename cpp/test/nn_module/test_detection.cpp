//
// Created by tunm on 2023/2/11.
//
#include "basic_types.h"
#include "../test_settings.h"
#include "opencv2/opencv.hpp"
#include "nn_implementation_module/detect/all.h"
#include "basic_types.h"
#include "utils.h"

using namespace hyper;

TEST_CASE("test_Detection", "[nn_detect]") {
    PRINT_SPLIT_LINE
    LOGD("[UnitTest]->Detect Model");

    std::string b_model_path = GET_DATA("models/r2_mobile/b320_backbone_h.mnn");
    std::string h_model_path = GET_DATA("models/r2_mobile/b320_header_h.mnn");

    cv::Mat test_image_1 = cv::imread(GET_DATA("images/pre.jpg"));
    CHECK(test_image_1.cols == 320);
    CHECK(test_image_1.rows == 320);

    SECTION("test_SplitDetectionSplitModel") {
        LOGD("Detect Model SplitModel");
        DetArch det;
        auto ret = det.Initialize(b_model_path, h_model_path, 320, 1);
        CHECK(ret == InferenceHelper::kRetOk);

        det.Detection(test_image_1);
        auto &result = det.m_results_;
        CHECK(result.size() == 1);

        auto &box = result[0];
        cv::Rect2f proposal_box(cv::Point(153, 205), cv::Point(183, 215));
        cv::Rect2f detected_box(cv::Point(box.x1, box.y1), cv::Point(box.x2, box.y2));
        auto iou = boundBoxOverlap(proposal_box, detected_box);
        CHECK(iou > 0.85);
    }

#if ENABLE_BENCHMARK_TEST
    SECTION("test_DetectionBenchmark") {
        LOGD("[UnitTest]->Detection Benchmark");
    }
#endif
}
