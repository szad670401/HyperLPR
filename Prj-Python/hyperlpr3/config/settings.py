import os
import sys

_MODEL_VERSION_ = "20240520"

if 'win32' in sys.platform:
    _DEFAULT_FOLDER_ = os.path.join(os.environ['HOMEPATH'], ".hyperlpr3")
else:
    _DEFAULT_FOLDER_ = os.path.join(os.environ['HOME'], ".hyperlpr3")

_ONLINE_URL_ = "http://hyperlpr.tunm.top/raw/"

onnx_runtime_config = dict(
    yolo_320x=os.path.join(_MODEL_VERSION_, "onnx", "y5f_m1_320.onnx"),
    yolo_640x=os.path.join(_MODEL_VERSION_, "onnx", "y5f_m1_640.onnx"),
    scrfd_320x=os.path.join(_MODEL_VERSION_, "onnx", "scrfd_plate_2.5g_bnkps_320x320.onnx"),
    scrfd_640x=os.path.join(_MODEL_VERSION_, "onnx", "scrfd_plate_2.5g_bnkps_640x640.onnx"),
    crnn_ctc_j18=os.path.join(_MODEL_VERSION_, "onnx", "crnn_ctc_rpv3_j18_b1.onnx"),
    crnn_ctc_rp3=os.path.join(_MODEL_VERSION_, "onnx", "crnn_ctc_rpv3p_r3.onnx"),
    simple_cls=os.path.join(_MODEL_VERSION_, "onnx", "litemodel_cls_96x_r1.onnx"),
)

onnx_model_maps = ["det_model_path_320x", "det_model_path_640x", "rec_model_path", "cls_model_path"]

_REMOTE_URL_ = "https://github.com/szad670401/HyperLPR/blob/master/resource/models/onnx/"