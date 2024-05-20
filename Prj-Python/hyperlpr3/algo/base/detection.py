import numpy as np
from .hamburger import HamburgerABC
from abc import ABCMeta, abstractmethod
from typing import List

class DetectionResult:
    def __init__(self, x1, y1, x2, y2, confidence, keypoints, layer=0):
        """
        Initialize a DetectionResult instance.

        :param x1: The x-coordinate of the top-left corner of the bounding box.
        :param y1: The y-coordinate of the top-left corner of the bounding box.
        :param x2: The x-coordinate of the bottom-right corner of the bounding box.
        :param y2: The y-coordinate of the bottom-right corner of the bounding box.
        :param confidence: The confidence score of the detection.
        :param keypoints: A list of keypoints, each represented by (x, y) coordinates.
        :param layer: The layer type of license plate has two types: single layer and double layer.
        """
        self.x1 = int(x1)
        self.y1 = int(y1)
        self.x2 = int(x2)
        self.y2 = int(y2)
        self.confidence = confidence
        self.keypoints = keypoints
        self.layer = layer

    def to_dict(self):
        """Convert the detection result to a dictionary."""
        return {
            'bounding_box': [self.x1, self.y1, self.x2, self.y2],
            'confidence': self.confidence,
            'keypoints': self.keypoints,
            'layer': self.layer
        }

    def __repr__(self):
        return f"DetectionResult(x1={self.x1}, y1={self.y1}, x2={self.x2}, y2={self.y2}, confidence={self.confidence}, keypoints={self.keypoints}, layer={self.layer})"


class BaseDetector(HamburgerABC, metaclass=ABCMeta):
    def __init__(self, input_size: tuple = None, model_name: str = None, *args, **kwargs):
        super().__init__(input_size, *args, **kwargs)
        self.model_name = model_name
        self.config = kwargs
        self._load_model()

    def _normalized_output(self, data):
        """
        Standardize the final detection result output for templating of various test frameworks.

        :param data: The postprocessed detection result.
        :return: The normalized detection result.
        """
        return data

    def detect(self, image: np.ndarray) -> List[DetectionResult]:
        """
        Public method to perform detection on an image.

        :param image: The input image.
        :return: The final standardized detection result.
        """
        flow = self._preprocess(image)
        flow = self._run_session(flow)
        result = self._postprocess(flow)
        final = self._normalized_output(result)
        return final

    def __call__(self, image, *args, **kwargs) -> List[DetectionResult]:
        return self.detect(image)

    @abstractmethod
    def _load_model(self):
        pass