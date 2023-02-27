from .config.settings import onnx_runtime_config as ort_cfg
from .inference.pipeline import LPRMultiTaskPipeline
from .common.typedef import *
from os.path import join
from .config.settings import _DEFAULT_FOLDER_


class LicensePlateCatcher(object):

    def __init__(self,
                 inference: int = INFER_ONNX_RUNTIME,
                 folder: str = _DEFAULT_FOLDER_,
                 detect_level: int = DETECT_LEVEL_LOW,
                 logger_level: int = 3):
        if inference == INFER_ONNX_RUNTIME:
            from hyperlpr3.inference.multitask_detect import MultiTaskDetectorORT
            from hyperlpr3.inference.recognition import PPRCNNRecognitionORT
            from hyperlpr3.inference.classification import ClassificationORT
            import onnxruntime as ort
            ort.set_default_logger_severity(logger_level)

            if detect_level == DETECT_LEVEL_LOW:
                # print(join(folder, ort_cfg['det_model_path_320x']))
                det = MultiTaskDetectorORT(join(folder, ort_cfg['det_model_path_320x']), input_size=(320, 320))
            elif detect_level == DETECT_LEVEL_HIGH:
                det = MultiTaskDetectorORT(join(folder, ort_cfg['det_model_path_640x']), input_size=(640, 640))
            else:
                raise NotImplemented
            rec = PPRCNNRecognitionORT(join(folder, ort_cfg['rec_model_path']), input_size=(48, 160))
            cls = ClassificationORT(join(folder, ort_cfg['cls_model_path']), input_size=(96, 96))
            self.pipeline = LPRMultiTaskPipeline(detector=det, recognizer=rec, classifier=cls)
        else:
            raise NotImplemented

    def __call__(self, image: np.ndarray, *args, **kwargs):
        return self.pipeline(image)
