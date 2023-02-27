import os
import sys

_MODEL_VERSION_ = "20230228"

if 'win32' in sys.platform:
    _DEFAULT_FOLDER_ = os.path.join(os.environ['HOMEPATH'], ".hyperlpr3")
else:
    _DEFAULT_FOLDER_ = os.path.join(os.environ['HOME'], ".hyperlpr3")

_ONLINE_URL_ = "https://tunm.oss-cn-hangzhou.aliyuncs.com/hyperlpr3/"

onnx_runtime_config = dict(
    det_model_path_320x=os.path.join(_MODEL_VERSION_, "onnx", "y5fu_320x_sim.onnx"),
    det_model_path_640x=os.path.join(_MODEL_VERSION_, "onnx", "y5fu_640x_sim.onnx"),
    rec_model_path=os.path.join(_MODEL_VERSION_, "onnx", "rpv3_mdict_160_r3.onnx"),
    cls_model_path=os.path.join(_MODEL_VERSION_, "onnx", "litemodel_cls_96x_r1.onnx"),
)

