//
// Created by tunm on 2023/1/25.
//

#ifndef ZEPHYRLPR_HYPER_LPR_SDK_INTERNAL_H
#define ZEPHYRLPR_HYPER_LPR_SDK_INTERNAL_H

#include "hyper_lpr_sdk.h"
#include "context_module/all.h"

typedef struct HLPR_Context {
    hyper::HyperLPRContext impl;
} HyperLPR_Context;

typedef struct HLPR_DataBuffer {
    hyper::CameraBuffer impl;
} HLPR_DataBuffer;

#endif //ZEPHYRLPR_HYPER_LPR_SDK_INTERNAL_H
