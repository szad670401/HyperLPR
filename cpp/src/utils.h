//
// Created by Tunm-Air13 on 2022/12/30.
//
#pragma once
#ifndef ZEPHYRLPR_UTILS_H
#define ZEPHYRLPR_UTILS_H

#include <unistd.h>

inline bool exists(const std::string &file_path) {
    return (access(file_path.c_str(), F_OK) != -1);
}

template<class ForwardIterator>
inline size_t argmin(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::min_element(first, last));
}

template<class ForwardIterator>
inline size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

inline double L2NormFor2Points(const cv::Point2f &p1, const cv::Point2f &p2) {
    auto r = pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2);

    return sqrt(r);
}

inline cv::Mat getRotateCropAndAlignMatrix(int img_crop_width, int img_crop_height,
                                           float *points2d, int point_num = 4) {
    assert(point_num == 4);
    float dst_array[] = {0, 0,
                         (float) img_crop_width, 0,
                         (float) img_crop_width, (float) img_crop_height,
                         0, (float) img_crop_height};
    cv::Mat src(4, 2, CV_32F, points2d);
    cv::Mat dst(4, 2, CV_32F);
    dst.data = (uchar *) dst_array;

    cv::Mat transform_matrix = cv::getPerspectiveTransform(src, dst);

    return transform_matrix;
}

inline void getRotateCropAndAlignPad(const cv::Mat &image, cv::Mat &out, float *points2d, int point_num = 4) {
    assert(point_num == 4);
    std::vector<cv::Point2f> vertex;
    for (int i = 0; i < point_num; ++i) {
        auto point = cv::Point2f(points2d[i * 2 + 0], points2d[i * 2 + 1]);
        vertex.push_back(point);
    }
    int img_crop_width = (int) std::max(L2NormFor2Points(vertex[0], vertex[1]), L2NormFor2Points(vertex[2], vertex[3]));
    int img_crop_height = (int) std::max(L2NormFor2Points(vertex[0], vertex[3]),
                                         L2NormFor2Points(vertex[1], vertex[2]));

    cv::Mat transform_matrix = getRotateCropAndAlignMatrix(img_crop_width, img_crop_height, points2d, point_num);
    cv::warpPerspective(image, out, transform_matrix, cv::Size(img_crop_width, img_crop_height), cv::INTER_CUBIC,
                        cv::BORDER_REPLICATE);

    if ((float) img_crop_height / (float) img_crop_width >= 1.5) {
        cv::rotate(out, out, cv::ROTATE_90_CLOCKWISE);
    }

}

inline void
imagePadding(const cv::Mat &image, cv::Mat &out, cv::Size target_size) {
    int ori_w = image.cols;
    int ori_h = image.rows;

    int w, h;
    float scale;
    if (ori_w > ori_h) {
        scale = (float) target_size.width / ori_w;
        w = target_size.width;
        h = ori_h * scale;
    } else {
        scale = (float) target_size.height / ori_h;
        h = target_size.height;
        w = ori_w * scale;
    }
    int wpad = std::max(target_size.width - w, 0);
    int hpad = std::max(target_size.height - h, 0);
    cv::Mat resized_img;
    cv::resize(image, resized_img, cv::Size(w, h));
    cv::copyMakeBorder(resized_img, out, 0, hpad, 0, wpad, cv::BORDER_CONSTANT, cv::Scalar(127.5f, 127.5f, 127.5f));
}

inline void
imagePadding(const cv::Mat &image, cv::Mat &out, float max_wh_ratio, cv::Size target_size,
             int limited_max_width = 160,
             int limited_min_width = 48) {
//    cv::imshow("w", image);
//    cv::waitKey(0);
//    std::cout << image.size << std::endl;
    int target_h = target_size.height;
    int target_w = target_size.width;
    max_wh_ratio = std::max(max_wh_ratio, (float) target_w / (float) target_h);
    target_w = (int) (max_wh_ratio * target_h);
    target_w = std::max(std::min(target_w, limited_max_width), limited_min_width);
    auto image_h = image.rows;
    auto image_w = image.cols;
    auto ratio = (float) image_w / image_h;
    int ratio_img_h = ceil(target_h * ratio);
    ratio_img_h = std::max(ratio_img_h, limited_min_width);
    int resized_w;
    if (ratio_img_h > target_w) {
        resized_w = target_w;
    } else {
        resized_w = int(ratio_img_h);
    }
    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(resized_w, target_h));


    int wpad = std::max(target_size.width - resized_w, 0);
    int hpad = std::max(target_size.height - target_h, 0);
//    std::cout << resized_w << "," << target_h << std::endl;
    cv::copyMakeBorder(resized_image, out, 0, hpad, 0, wpad, cv::BORDER_CONSTANT, cv::Scalar(127.5f, 127.5f, 127.5f));


}

template<class T>
inline float xyxyArea(T x1, T y1, T x2, T y2) {
    auto w = x2 - x1;
    auto h = y2 - y1;

    return (float) (w * h);
}

template<class T>
inline float boundBoxOverlap(const cv::Rect_<T> &box1, const cv::Rect_<T> &box2) {
    if (box1.x > box2.x + box2.width) { return 0.0; }
    if (box1.y > box2.y + box2.height) { return 0.0; }
    if (box1.x + box1.width < box2.x) { return 0.0; }
    if (box1.y + box1.height < box2.y) { return 0.0; }
    T colInt = std::min(box1.x + box1.width, box2.x + box2.width) - std::max(box1.x, box2.x);
    T rowInt = std::min(box1.y + box1.height, box2.y + box2.height) - std::max(box1.y, box2.y);
    T intersection = colInt * rowInt;
    T area1 = box1.width * box1.height;
    T area2 = box2.width * box2.height;

    return intersection / (area1 + area2 - intersection);
}

inline cv::Mat plateSqueeze(const cv::Mat& img) {
    int height = img.rows;
    int width = img.cols;
    int channels = img.channels();

    int upper_bound = static_cast<int>(0.416 * height);
    int lower_bound = static_cast<int>(0.333 * height);

    cv::Mat upper_part = img(cv::Rect(0, 0, width, upper_bound));
    cv::Mat lower_part = img(cv::Rect(0, lower_bound, width, height - lower_bound));

    cv::Mat upper_part_resized;
    cv::resize(upper_part, upper_part_resized, cv::Size(width, lower_part.rows));

    cv::Mat merged_img;
    cv::hconcat(upper_part_resized, lower_part, merged_img);

    return merged_img;
} 

#endif //ZEPHYRLPR_UTILS_H
