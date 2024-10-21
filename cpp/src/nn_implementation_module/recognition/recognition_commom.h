//
// Created by Tunm-Air13 on 2022/12/30.
//

#ifndef ZEPHYRLPR_RECOGNITION_COMMOM_H
#define ZEPHYRLPR_RECOGNITION_COMMOM_H

#include "basic_types.h"

namespace hyper {

typedef struct TextLine {
    IndexList char_index;                             ///< Index list of each character
    std::vector<float> char_scores;                   ///< Confidence list of each character
    float average_score;                              ///< Average confidence
    std::string code;                                 ///< License plate string
};

typedef std::vector<TextLine> TextLines;             ///< Multiline text presentation

}

#endif //ZEPHYRLPR_RECOGNITION_COMMOM_H
