//
// Created by tunm on 2023/1/25.
//

#include "hyper_lpr_sdk_internal.h"
#include "hyper_lpr_sdk.h"
#include "context_module/all.h"


P_HLPR_DataBuffer HLPR_CreateDataBufferEmpty() {
    return new HLPR_DataBuffer();
}

HREESULT HLPR_DataBufferSetData(
        P_HLPR_DataBuffer buffer,               // [in] CameraStream handle - 相机流组件的句柄指针
        const uint8_t *data,                    // [in] Raw data stream - 原始的数据流
        int width,                              // [in] Image width - 图像宽度
        int height                              // [in] Image height - 图像高度
) {
    buffer->impl.SetDataBuffer(data, height, width);

    return hyper::hRetOk;
}

HREESULT HLPR_DataBufferSetRotationMode(
        P_HLPR_DataBuffer buffer,              // [in] CameraStream handle - 相机流组件的句柄指针
        HLPR_Rotation mode                     // [in] CameraRotation mode - 相机流组件旋转模式
) {
    if (mode == 1) {
        buffer->impl.SetRotationMode(hyper::ROTATION_90);
    } else if (mode == 2) {
        buffer->impl.SetRotationMode(hyper::ROTATION_180);
    } else if (mode == 3) {
        buffer->impl.SetRotationMode(hyper::ROTATION_270);
    } else {
        buffer->impl.SetRotationMode(hyper::ROTATION_0);
    }

    return hyper::hRetOk;
}

HREESULT HLPR_DataBufferSetStreamFormat(
        P_HLPR_DataBuffer buffer,            // [in] CameraStream handle - 相机流组件的句柄指针
        HLPR_ImageFormat mode                       // [in] CameraRotation data format - 相机流组件数据格式
) {
    // STREAM_RGB = 0,
    // STREAM_BGR = 1,
    // STREAM_RGBA = 2,
    // STREAM_BGRA = 3,
    // STREAM_YUV_NV12 = 4,
    // STREAM_YUV_NV21 = 5,
    if (mode == 0) {
        buffer->impl.SetDataFormat(hyper::RGB);
    } else if (mode == 1) {
        buffer->impl.SetDataFormat(hyper::BGR);
    } else if (mode == 2) {
        buffer->impl.SetDataFormat(hyper::RGBA);
    } else if (mode == 3) {
        buffer->impl.SetDataFormat(hyper::BGRA);
    } else if (mode == 4) {
        buffer->impl.SetDataFormat(hyper::NV12);
    } else if (mode == 5) {
        buffer->impl.SetDataFormat(hyper::NV21);
    }

    return hyper::hRetOk;
}

P_HLPR_DataBuffer HLPR_CreateDataBuffer(P_HLPR_ImageData data) {
    auto buffer = new HLPR_DataBuffer();
    if (data->rotation == 1) {
        buffer->impl.SetRotationMode(hyper::ROTATION_90);
    } else if (data->rotation == 2) {
        buffer->impl.SetRotationMode(hyper::ROTATION_180);
    } else if (data->rotation == 3) {
        buffer->impl.SetRotationMode(hyper::ROTATION_270);
    } else {
        buffer->impl.SetRotationMode(hyper::ROTATION_0);
    }
    if (data->format == 0) {
        buffer->impl.SetDataFormat(hyper::RGB);
    } else if (data->format == 1) {
        buffer->impl.SetDataFormat(hyper::BGR);
    } else if (data->format == 2) {
        buffer->impl.SetDataFormat(hyper::RGBA);
    } else if (data->format == 3) {
        buffer->impl.SetDataFormat(hyper::BGRA);
    } else if (data->format == 4) {
        buffer->impl.SetDataFormat(hyper::NV12);
    } else if (data->format == 5) {
        buffer->impl.SetDataFormat(hyper::NV21);
    }
    buffer->impl.SetDataBuffer(data->data, data->height, data->width);

    return buffer;
}

HREESULT HLPR_ReleaseDataBuffer(P_HLPR_DataBuffer buffer) {
    delete buffer;

    return hyper::hRetOk;
}


P_HLPR_Context HLPR_CreateContext(P_HLPR_ContextConfiguration configuration) {
    auto ctx = new HLPR_Context();
    ctx->impl.Initialize(
            configuration->models_path,
            configuration->max_num,
            hyper::DetectLevel(configuration->det_level),
            configuration->threads,
            configuration->use_half,
            configuration->box_conf_threshold,
            configuration->nms_threshold,
            configuration->rec_confidence_threshold);

    return ctx;
}


HREESULT HLPR_ContextUpdateStream(P_HLPR_Context ctx, P_HLPR_DataBuffer buffer, P_HLPR_PlateResultList results) {
    ctx->impl(buffer->impl);
    auto &list = ctx->impl.getMObjectResults();
    results->plate_size = list.size();
    results->plates = (P_HLPR_PlateResult) list.data();

    return hyper::hRetOk;
}

HREESULT HLPR_ContextQueryStatus(P_HLPR_Context ctx) {
    return ctx->impl.getMInitStatus();
}


HREESULT HLPR_ReleaseContext(P_HLPR_Context ctx) {
    delete ctx;

    return hyper::hRetOk;
}


//HREESULT HLPR_DataBufferTest(P_HLPR_DataBuffer buffer, const char *save_path) {
//    cv::Mat image = buffer->impl.GetScaledImage(1.0f, true);
//    cv::imwrite(save_path, image);
//
//    return 0;
//}