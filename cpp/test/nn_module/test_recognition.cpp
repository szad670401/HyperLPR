//
// Created by tunm on 2023/2/11.
//
#include "basic_types.h"
#include "../test_settings.h"
#include "opencv2/opencv.hpp"
#include "nn_implementation_module/recognition/all.h"
#include "basic_types.h"
#include "utils.h"

using namespace hyper;

TEST_CASE("test_Recognition", "[nn_rec]") {
    PRINT_SPLIT_LINE
    LOGD("[UnitTest]->Recognition Model");

    std::string model_path = GET_DATA("models/r2_mobile/rpv3_mdict_160h.mnn");

    std::vector<std::string> predict_images_list = {
            GET_DATA("images/rec_crop/_0_津B6H920.jpg"),
            GET_DATA("images/rec_crop/_1_皖KD01833.jpg"),
            GET_DATA("images/rec_crop/_6_蒙B023H6.jpg"),
            GET_DATA("images/rec_crop/_8_冀D5L690.jpg"),
    };

    std::vector<std::string> predict_results_code = {
            "津B6H920", "皖KD01833", "蒙B023H6", "冀D5L690",
    };

    RecognitionEngine recEngine;
    auto ret = recEngine.Initialize(model_path);
    CHECK(ret == InferenceHelper::kRetOk);

    SECTION("test_SplitDetectionSplitModel") {
        LOGD("Rec Model RPV3");
        for (int i = 0; i < predict_images_list.size(); ++i) {
            cv::Mat img = cv::imread(predict_images_list[i]);
            CHECK(!img.empty());
            float wh_ratio = (float) img.cols / img.rows;
            cv::Mat align_image_pad;
            imagePadding(img, align_image_pad, wh_ratio, recEngine.getMInputImageSize());
            TextLine line;
            ret = recEngine.Inference(align_image_pad, line);
            LOGD("%s -> %s", predict_results_code[i].c_str(), line.code.c_str());
            CHECK(ret == InferenceHelper::kRetOk);
            CHECK(strcmp(predict_results_code[i].c_str(), line.code.c_str()) == 0);
        }

    }

}