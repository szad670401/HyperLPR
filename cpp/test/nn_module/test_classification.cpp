//
// Created by tunm on 2023/2/11.
//


#include "basic_types.h"
#include "../test_settings.h"
#include "opencv2/opencv.hpp"
#include "nn_implementation_module/classification/all.h"
#include "basic_types.h"
#include "utils.h"

using namespace hyper;

TEST_CASE("test_Classification", "[nn_cls]") {
    PRINT_SPLIT_LINE
    LOGD("[UnitTest]->Classification Model");

    std::string model_path = GET_DATA("models/r2_mobile/litemodel_cls_96xh.mnn");

    std::vector<std::string> predict_images_list = {
            GET_DATA("images/align/1.jpg"),
            GET_DATA("images/align/3.jpg"),
            GET_DATA("images/align/5.jpg"),
    };
    std::vector<PlateColor> predict_results_cls = {
            PlateColor::BLUE, PlateColor::YELLOW, PlateColor::GREEN,
    };
    std::vector<float> predict_results_confidence = {
            0.9999293f, 0.8975975f, 0.9997952f
    };

    CHECK(predict_results_confidence.size() == predict_results_cls.size());
    CHECK(predict_results_confidence.size() == predict_images_list.size());

    ClassificationEngine clsEngine;
    auto ret = clsEngine.Initialize(model_path, cv::Size_<int>(96, 96));
    CHECK(ret == InferenceHelper::kRetOk);

    SECTION("test_ClassificationModelPredict") {
        for (int i = 0; i < predict_images_list.size(); ++i) {
            cv::Mat img = cv::imread(predict_images_list[i]);
            CHECK(!img.empty());
            CHECK(img.cols == 96);
            CHECK(img.rows == 96);
            ret = clsEngine.Inference(img);
            CHECK(ret == InferenceHelper::kRetOk);
            CHECK(PlateColor(clsEngine.getMOutputColor()) == predict_results_cls[i]);
            CHECK(clsEngine.getMOutputMaxConfidence() == Approx(predict_results_confidence[i]).epsilon(0.001));
        }
    }


}