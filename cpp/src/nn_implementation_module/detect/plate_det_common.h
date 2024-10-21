//
// Created by tunm on 2022/12/29.
//
#pragma once
#ifndef ZEPHYRLPR_PLATE_DET_COMMON_H
#define ZEPHYRLPR_PLATE_DET_COMMON_H

#include <algorithm>

namespace hyper {

enum LayersNum {
    MONO,       ///< 单层车牌
    DOUBLE,     ///< 双层车牌
};

typedef struct PlateLocation {
    float x1;                      ///< 左上角点x坐标
    float y1;                      ///< 左上角点y坐标
    float x2;                      ///< 右下角点x坐标
    float y2;                      ///< 右下角点y坐标
    float det_confidence;        ///< 目标检测的置信度
    float kps[8];                ///< 四个角的角点信息
    LayersNum layers;         ///< 车牌层级类型
};


inline void nms(std::vector<PlateLocation> &input_plates, float nms_threshold) {
    std::sort(input_plates.begin(), input_plates.end(),
              [](PlateLocation a, PlateLocation b) { return a.det_confidence > b.det_confidence; });
    std::vector<float> area(input_plates.size());
    for (int i = 0; i < int(input_plates.size()); ++i) {
        area[i] =
                (input_plates.at(i).x2 - input_plates.at(i).x1 + 1) *
                (input_plates.at(i).y2 - input_plates.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_plates.size()); ++i) {
        for (int j = i + 1; j < int(input_plates.size());) {
            float xx1 = (std::max)(input_plates[i].x1, input_plates[j].x1);
            float yy1 = (std::max)(input_plates[i].y1, input_plates[j].y1);
            float xx2 = (std::min)(input_plates[i].x2, input_plates[j].x2);
            float yy2 = (std::min)(input_plates[i].y2, input_plates[j].y2);
            float w = (std::max)(float(0), xx2 - xx1 + 1);
            float h = (std::max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (area[i] + area[j] - inter);
            if (ovr >= nms_threshold) {
                input_plates.erase(input_plates.begin() + j);
                area.erase(area.begin() + j);
            } else {
                j++;
            }
        }
    }
}

inline std::vector<float> xywh2xyxy(float x, float y, float w, float h) {
    float x1 = x - w / 2;
    float y1 = y - h / 2;
    float x2 = x + w / 2;
    float y2 = y + h / 2;
    return {x1, y1, x2, y2};
}

}

#endif //ZEPHYRLPR_PLATE_DET_COMMON_H
