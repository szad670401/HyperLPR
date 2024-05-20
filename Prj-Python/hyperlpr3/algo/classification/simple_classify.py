from hyperlpr3.algo.base.classification import BaseClassification, ClassificationResult
import onnxruntime as ort
import numpy as np
import cv2

_class_names = ["blue", "green", "yellow"]


def encode_images(image: np.ndarray):
    """
    Encode images by normalizing and transposing.

    Args:
        image (np.ndarray): Input image.

    Returns:
        np.ndarray: Encoded image.
    """
    image_encode = image / 255.0
    if len(image_encode.shape) == 4:
        image_encode = image_encode.transpose(0, 3, 1, 2)
    else:
        image_encode = image_encode.transpose(2, 0, 1)
    image_encode = image_encode.astype(np.float32)

    return image_encode


class SimpleClassification(BaseClassification):
    def __init__(self, onnx_path, *args, **kwargs):
        """
        Initialize the SimpleClassification class.

        :param onnx_path: Path to the ONNX model.
        :param class_names: List of class names for classification.
        :param args: Additional positional arguments.
        :param kwargs: Additional keyword arguments.
        """
        self.onnx_path = onnx_path
        super().__init__(class_names=_class_names, *args, **kwargs)

    def _load_model(self):
        """Load the ONNX model."""
        self.session = ort.InferenceSession(self.onnx_path, None)
        self.input_config = self.session.get_inputs()[0]
        self.output_config = self.session.get_outputs()[0]
        self.input_size = tuple(self.input_config.shape[2:])
        self.running = True

    def _run_session(self, data):
        """
        Run the ONNX model inference session.

        :param data: Preprocessed input data.
        :return: Model inference output.
        """
        result = self.session.run([self.output_config.name], {self.input_config.name: data})
        return result[0]

    def _postprocess(self, data):
        """
        Postprocess the model output.

        :param data: Model output data.
        :return: Processed output data.
        """
        return data

    def _preprocess(self, image):
        """
        Preprocess the input image.

        :param image: Input image.
        :return: Preprocessed image.
        """
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single image."
        image_resize = cv2.resize(image, self.input_size)
        encode = encode_images(image_resize)
        input_tensor = np.expand_dims(encode, 0)
        return input_tensor

    def _normalized_output(self, data):
        """
        Standardize the final classification result output.

        :param data: The postprocessed classification result.
        :return: A ClassificationResult instance containing the class name, class index, and confidence.
        """
        class_index = np.argmax(data)
        confidence = np.max(data)
        class_name = self.class_names[class_index] if self.class_names else str(class_index)
        return ClassificationResult(class_name, class_index, confidence)
