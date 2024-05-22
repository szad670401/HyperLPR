from .config.settings import onnx_runtime_config as ort_cfg
from .algo.pipeline import UsualFlow, DevFlow
from .algo.pallet import *
from os.path import join
from .config.settings import _DEFAULT_FOLDER_
from .config.configuration import initialization

initialization()

class LicensePlateCatcher(object):
    """
    LicensePlateCatcher is a class for license plate detection, recognition, and classification.

    Parameters:
    - scheme (int): Algorithm scheme, default is ALGO_SCHEME_USUAL.
    - folder (str): Folder path where model files are stored, default is _DEFAULT_FOLDER_.
    - detect_level (int): Detection level, default is DETECT_LEVEL_LOW.
    - logger_level (int): ONNX Runtime logger severity level, default is 3.
    - full_result (bool): Whether to return the full result, default is False.
    """

    def __init__(self,
                 scheme: int = ALGO_SCHEME_USUAL,
                 folder: str = _DEFAULT_FOLDER_,
                 detect_level: int = DETECT_LEVEL_LOW,
                 logger_level: int = 3,
                 full_result: bool = False,
                 providers=['CPUExecutionProvider']):
        if scheme == ALGO_SCHEME_USUAL:
            from hyperlpr3.algo.detection.yolo import YoloMultiTaskDetector
            from hyperlpr3.algo.recognition.crnn_ctc import CRNNCTCRecognizer
            from hyperlpr3.algo.classification.simple_classify import SimpleClassification
            import onnxruntime as ort
            ort.set_default_logger_severity(logger_level)

            if detect_level == DETECT_LEVEL_LOW:
                det = YoloMultiTaskDetector(join(folder, ort_cfg['yolo_320x']), providers=providers)
            elif detect_level == DETECT_LEVEL_HIGH:
                det = YoloMultiTaskDetector(join(folder, ort_cfg['yolo_640x']), providers=providers)
            else:
                raise NotImplementedError("Unsupported detection level.")

            rec = CRNNCTCRecognizer(join(folder, ort_cfg['crnn_ctc_j18']), providers=providers)
            cls = SimpleClassification(join(folder, ort_cfg['simple_cls']), providers=providers)
            self.pipeline = UsualFlow(detector=det, recognizer=rec, classifier=cls, full_result=full_result)

        elif scheme == ALGO_SCHEME_DEV:
            from hyperlpr3.algo.detection.yolo import YoloMultiTaskDetector
            from hyperlpr3.algo.recognition.crnn_ctc import CRNNCTCRecognizer
            from hyperlpr3.algo.classification.simple_classify import SimpleClassification
            import onnxruntime as ort
            ort.set_default_logger_severity(logger_level)

            if detect_level == DETECT_LEVEL_LOW:
                det = YoloMultiTaskDetector(join(folder, ort_cfg['yolo_320x']), providers=providers)
            elif detect_level == DETECT_LEVEL_HIGH:
                det = YoloMultiTaskDetector(join(folder, ort_cfg['yolo_640x']), providers=providers)
            else:
                raise NotImplementedError("Unsupported detection level.")

            rec = CRNNCTCRecognizer(join(folder, ort_cfg['crnn_ctc_rp3']), providers=providers)
            cls = SimpleClassification(join(folder, ort_cfg['simple_cls']), providers=providers)
            self.pipeline = DevFlow(detector=det, recognizer=rec, classifier=cls, full_result=full_result)
        else:
            raise NotImplementedError("Unsupported scheme.")

    def __call__(self, image: np.ndarray, *args, **kwargs):
        """
        Processes an input image to detect, recognize, and classify license plates.

        Parameters:
        - image (np.ndarray): Input image.

        Returns:
        - The result of the pipeline processing.
        """
        return self.pipeline(image)
