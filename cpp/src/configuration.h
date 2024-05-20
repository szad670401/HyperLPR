//
// Created by Tunm-Air13 on 2023/2/21.
//
#pragma once
#ifndef ZEPHYRLPR_CONFIGURATION_H
#define ZEPHYRLPR_CONFIGURATION_H

#include "basic_types.h"
#include <iostream>
#include "log.h"

namespace hyper {

const std::string DETECT_LOW_FILENAME = "y5f_m1_320.mnn";
const std::string DETECT_HIGH_FILENAME = "y5f_m1_640.mnn";
const std::string CLS_MODEL_FILENAME = "litemodel_cls_96xh.mnn";
const std::string REC_MODEL_FILENAME = "rpv3_mdict_160h.mnn";

const InputSize REC_INPUT_SIZE = {160, 48};
const InputSize CLS_INPUT_SIZE = {96, 96};

const int REC_MAX_CHAR_NUM = 20;
const int REC_CHAR_CLASS_NUM = 78;

}

#endif //ZEPHYRLPR_CONFIGURATION_H
