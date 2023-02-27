//
// Created by Tunm-Air13 on 2022/12/30.
//

#ifndef ZEPHYRLPR_RECOGNITION_COMMOM_H
#define ZEPHYRLPR_RECOGNITION_COMMOM_H

#include "basic_types.h"

namespace hyper {

typedef struct TextLine {
    IndexList char_index;                             ///< 索引列表
    std::vector<float> char_scores;                   ///< 置信度列表
    float average_score;                              ///< 平均置信度
    std::string code;                                 ///< 车牌字符串
};

typedef std::vector<TextLine> TextLines;             ///< 多行文本表达

}

#endif //ZEPHYRLPR_RECOGNITION_COMMOM_H
