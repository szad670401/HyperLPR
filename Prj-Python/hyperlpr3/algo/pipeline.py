from loguru import logger
from .base import *
from .pallet import *
from .utils.image_process import *


class UsualFlow(object):

    def __init__(self, detector: BaseDetector, recognizer: BaseTextRecognizer, classifier: BaseClassification,
                 full_result=False):
        self.detector = detector
        if not self.detector.is_running():
            logger.error("The detector may not be running.")
        self.recognizer = recognizer
        if not self.recognizer.is_running():
            logger.error("The recognizer may not be running.")
        self.classifier = classifier
        if not self.classifier.is_running():
            logger.error("The classifier may not be running.")
        self.full_result = full_result

    def run(self, image: np.ndarray):
        result = list()
        if len(image.shape) != 3:
            logger.error("Input image must be 3 channels.")
            return result
        if image is None:
            logger.error("Input image cannot be empty.")
            return result
        outputs = self.detector(image)
        for out in outputs:
            lmk = out.keypoints
            pad = get_rotate_crop_image(image, lmk)
            rect = [out.x1, out.y1, out.x2, out.y2]
            if out.layer == DOUBLE:
                pad = plate_squeeze(pad)
            text_pred = self.recognizer(pad)
            if text_pred.text == "":
                continue
            if len(text_pred.text) >= 7:
                plate_type = code_filter(text_pred.text)
                if plate_type == UNKNOWN:
                    cls = self.classifier(pad)
                    idx = cls.class_index
                    if idx == PLATE_TYPE_YELLOW:
                        if out.layer == DOUBLE:
                            plate_type = YELLOW_DOUBLE
                        else:
                            plate_type = YELLOW_SINGLE
                    elif idx == PLATE_TYPE_BLUE:
                        plate_type = BLUE
                    elif idx == PLATE_TYPE_GREEN:
                        plate_type = GREEN
                plate = Plate(vertex=out.keypoints, plate_code=text_pred.text, det_bound_box=np.asarray(rect),
                              rec_confidence=text_pred.avg_confidence, dex_bound_confidence=out.confidence,
                              plate_type=plate_type)
                if self.full_result:
                    result.append(plate.to_full_result())
                else:
                    result.append(plate.to_result())

        return result

    def __call__(self, image: np.ndarray, *args, **kwargs):
        return self.run(image)

