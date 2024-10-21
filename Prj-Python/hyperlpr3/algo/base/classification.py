import numpy as np
from hyperlpr3.algo.utils.image_process import hyperlpr_cost
from .hamburger import HamburgerABC
from abc import ABCMeta, abstractmethod


class ClassificationResult:
    def __init__(self, class_name, class_index, confidence):
        """
        Initialize a ClassificationResult instance.

        :param class_name: The name of the classified category.
        :param class_index: The index of the classified category.
        :param confidence: The confidence score of the classification.
        """
        self.class_name = class_name
        self.class_index = class_index
        self.confidence = confidence

    def to_dict(self):
        """Convert the classification result to a dictionary."""
        return {
            'class_name': self.class_name,
            'class_index': self.class_index,
            'confidence': self.confidence
        }

    def __repr__(self):
        return f"ClassificationResult(class_name={self.class_name}, class_index={self.class_index}, confidence={self.confidence})"


class BaseClassification(HamburgerABC, metaclass=ABCMeta):
    def __init__(self, input_size: tuple = None, model_name: str = None, class_names: list = None, *args, **kwargs):
        """
        Initialize the BaseClassification class.

        :param input_size: Tuple indicating the input size (e.g., (width, height)).
        :param model_name: Name of the model.
        :param class_names: List of class names for classification.
        :param args: Additional positional arguments.
        :param kwargs: Additional keyword arguments.
        """
        super().__init__(input_size, *args, **kwargs)
        self.model_name = model_name
        self.class_names = class_names
        self.config = kwargs
        self._load_model()

    @abstractmethod
    def _load_model(self):
        """Load the classification model. This method should be implemented by subclasses."""
        pass

    @abstractmethod
    def _run_session(self, data):
        """Run the session with the loaded model. This method should be implemented by subclasses."""
        pass

    @abstractmethod
    def _postprocess(self, data):
        """Postprocess the output of the session. This method should be implemented by subclasses."""
        pass

    @abstractmethod
    def _preprocess(self, image):
        """Preprocess the input image. This method should be implemented by subclasses."""
        pass

    def _normalized_output(self, data) -> ClassificationResult:
        """
        Standardize the final classification result output.

        :param data: The postprocessed classification result.
        :return: A ClassificationResult instance containing the class name, class index, and confidence.
        """
        pass

    @hyperlpr_cost("PlateColorClassification")
    def classify(self, image: np.ndarray) -> ClassificationResult:
        """
        Public method to perform classification on an image.

        :param image: The input image.
        :return: The final standardized classification result.
        """
        flow = self._preprocess(image)
        flow = self._run_session(flow)
        result = self._postprocess(flow)
        final = self._normalized_output(result)
        return final

    def __call__(self, image: np.ndarray, *args, **kwargs) -> ClassificationResult:
        return self.classify(image)
