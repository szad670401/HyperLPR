//
// Created by Tunm-Air13 on 2023/2/8.
//

#include <iostream>
#include "opencv2/opencv.hpp"
//#include "loader_module/all.h"
#include "nn_implementation_module/all.h"
#include "configuration.h"

using namespace hyper;

int main(int argc, char **argv) {
    char *model_path = argv[1];
    char *image_path = argv[2];

    int input_size = 320;
    std::string backbone_path = std::string(model_path) + "/" + hyper::DETECT_LOW_BACKBONE_FILENAME;
    std::string head_path = std::string(model_path) + "/" + hyper::DETECT_LOW_HEAD_FILENAME;

//    std::string backbone_path = std::string(model_path) + "/" + hyper::DETECT_HIGH_BACKBONE_FILENAME;
//    std::string head_path = std::string(model_path) + "/" + hyper::DETECT_HIGH_HEAD_FILENAME;
//    int input_size = 640;

    cv::Mat image = cv::imread(image_path);

//    DetBackbone backbone;
//    backbone.Initialize(backbone_model);
//
//    DetHeader header;
//    header.Initialize(header_model);
//
//
//    backbone.Inference(image);
//
//    header.Inference(backbone.getMOutputTensorInfoList()[0].GetDataAsFloat(),
//                     backbone.getMOutputTensorInfoList()[1].GetDataAsFloat(),
//                     backbone.getMOutputTensorInfoList()[1].GetDataAsFloat());


//    for (int i = 0; i < 20; ++i) {
//        std::cout << header.getMOutputTensorInfoList()[0].GetDataAsFloat()[i] << std::endl;
//    }


//    float *h = header.getMOutputTensorInfoList()[0].GetDataAsFloat();
//    for (int i = 0; i < 6300 * 15; ++i) {
////        std::cout << backbone.m_output_feature_map_40p_.get()[i] << std::endl;
//        FILE *fp = NULL;
//        fp = fopen("head.txt", "a");
//        fprintf(fp, "%f\n", h[i]);
//        fclose(fp);
//    }
//


//    float *p40 = backbone.getMOutputTensorInfoList()[0].GetDataAsFloat();
//    for (int i = 0; i < 45 * 40 *40; ++i) {
////        std::cout << backbone.m_output_feature_map_40p_.get()[i] << std::endl;
//        FILE *fp = NULL;
//        fp = fopen("40.txt", "a");
//        fprintf(fp, "%f\n", p40[i]);
//        fclose(fp);
//    }
//
//    std::cout << std::endl;
//    float *p20 = backbone.getMOutputTensorInfoList()[1].GetDataAsFloat();
//    for (int i = 0; i < 45 * 20 * 20; ++i) {
////        std::cout << backbone.m_output_feature_map_20p_.get()[i] << std::endl;
//        FILE *fp = NULL;
//        fp = fopen("20.txt", "a");
//        fprintf(fp, "%f\n", p20[i]);
//        fclose(fp);
//    }
//    std::cout << std::endl;
//    float *p10 = backbone.getMOutputTensorInfoList()[2].GetDataAsFloat();
//    for (int i = 0; i < 45 * 10 * 10; ++i) {
////        std::cout << backbone.m_output_feature_map_10p_.get()[i] << std::endl;
//        FILE *fp = NULL;
//        fp = fopen("10.txt", "a");
//        fprintf(fp, "%f\n", p10[i]);
//        fclose(fp);
//    }

    DetArch arch;
    arch.Initialize(backbone_path, head_path, input_size);
    double time;
    time = (double)cv::getTickCount();
    arch.Detection(image, true);
    time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();

    auto &results = arch.m_results_;

    for (auto &plate : results) {
        std::cout << plate.x1 << ", " << plate.y1 << std::endl;
        cv::rectangle(image, cv::Point2f(plate.x1, plate.y1), cv::Point2f(plate.x2, plate.y2),
                      cv::Scalar(100, 100, 200), 1);
        for (int i = 0; i < 4; ++i) {
            cv::line(image, cv::Point2f(plate.kps[i * 2 + 0], plate.kps[i * 2 + 1]),
                     cv::Point2f(plate.kps[i * 2 + 0], plate.kps[i * 2 + 1]), cv::Scalar(100, 220, 20), 1);
        }

    }
#ifdef BUILD_LINUX_ARM7
    cv::imwrite("out.jpg", image);
#else
    cv::imshow("w", image);
    cv::waitKey(0);
#endif

    return 0;

}