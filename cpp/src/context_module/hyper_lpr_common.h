//
// Created by tunm on 2023/1/25.
//
#pragma once
#ifndef ZEPHYRLPR_HYPER_LPR_COMMON_H
#define ZEPHYRLPR_HYPER_LPR_COMMON_H

#include "nn_implementation_module/all.h"

namespace hyper {

enum PlateType {
    UNKNOWN = -1,                         ///< 未知车牌
    BLUE = 0,                             ///< 蓝牌
    YELLOW_SINGLE = 1,                    ///< 黄牌单层
    WHILE_SINGLE = 2,                     ///< 白牌单层
    GREEN = 3,                            ///< 绿牌新能源
    BLACK_HK_MACAO = 4,                   ///< 黑牌港澳
    HK_SINGLE = 5,                        ///< 香港单层
    HK_DOUBLE = 6,                        ///< 香港双层
    MACAO_SINGLE = 7,                     ///< 澳门单层
    MACAO_DOUBLE = 8,                     ///< 澳门双层
    YELLOW_DOUBLE = 9,                    ///< 黄牌双层
};

enum DetectLevel {
    DETECT_LEVEL_LOW = 0,
    DETECT_LEVEL_HIGH,
};

typedef struct PlateObject {
    PlateLocation location{};
    TextLine line{};
    PlateColor type{};

} PlateObject;

typedef struct PlateResult {
    float x1;
    float y1;
    float x2;
    float y2;
    PlateType type;
    float text_confidence;
    char code[128];
} PlateResult;

typedef std::vector<PlateObject> PlateObjectList;

typedef std::vector<PlateResult> PlateResultList;

}

#endif //ZEPHYRLPR_HYPER_LPR_COMMON_H
