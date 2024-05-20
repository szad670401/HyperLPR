import numpy as np

from .hamburger import HamburgerABC
from abc import ABCMeta, abstractmethod


class TextRecognitionResult:
    def __init__(self, text, avg_confidence, char_confidences):
        """
        Initialize a TextRecognitionResult instance.

        :param text: The recognized text string.
        :param avg_confidence: The average confidence score of the recognized text.
        :param char_confidences: A list of confidence scores for each character in the recognized text.
        """
        self.text = text
        self.avg_confidence = avg_confidence
        self.char_confidences = char_confidences

    def to_dict(self):
        """Convert the text recognition result to a dictionary."""
        return {
            'text': self.text,
            'avg_confidence': self.avg_confidence,
            'char_confidences': self.char_confidences
        }

    def __repr__(self):
        return f"TextRecognitionResult(text={self.text}, avg_confidence={self.avg_confidence}, char_confidences={self.char_confidences})"


class BaseTextRecognizer(HamburgerABC, metaclass=ABCMeta):
    def __init__(self, input_size: tuple = None, model_name: str = None, *args, **kwargs):
        """
        Initialize the BaseTextRecognizer class.

        :param input_size: Tuple indicating the input size (e.g., (width, height)).
        :param model_name: Name of the model.
        :param args: Additional positional arguments.
        :param kwargs: Additional keyword arguments.
        """
        super().__init__(input_size, *args, **kwargs)
        self.input_size = input_size
        self.model_name = model_name
        self.config = kwargs
        self._load_model()

    def _normalized_output(self, data):
        """
        Standardize the final text recognition result output.

        :param data: The postprocessed recognition result.
        :return: A TextRecognitionResult instance containing the recognized text, average confidence, and character confidences.
        """
        text = ''.join([chr(c) for c in data['text']])
        avg_confidence = sum(data['confidences']) / len(data['confidences'])
        char_confidences = data['confidences']
        return TextRecognitionResult(text, avg_confidence, char_confidences)

    def recognize(self, image: np.ndarray):
        """
        Public method to perform text recognition on an image.

        :param image: The input image.
        :return: The final standardized text recognition result.
        """
        flow = self._preprocess(image)
        flow = self._run_session(flow)
        result = self._postprocess(flow)
        final = self._normalized_output(result)
        return final

    def __call__(self, image: np.ndarray, *args, **kwargs) -> TextRecognitionResult:
        return self.recognize(image)

    @abstractmethod
    def _load_model(self):
        pass
