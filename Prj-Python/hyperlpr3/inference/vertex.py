import cv2
import numpy as np
from .base.base import HamburgerABC
from hyperlpr3.common.tools_process import cost


def encode_images(image: np.ndarray):
    image_encode = image / 255.0
    if len(image_encode.shape) == 4:
        image_encode = image_encode.transpose(0, 3, 1, 2)
    else:
        image_encode = image_encode.transpose(2, 0, 1)
    image_encode = image_encode.astype(np.float32)

    return image_encode


class BVTVertexMNN(HamburgerABC):

    def __init__(self, mnn_path, *args, **kwargs):
        from .common.mnn_adapt import MNNAdapter
        super().__init__(*args, **kwargs)
        self.input_shape = (1, 3, self.input_size[0], self.input_size[1])
        self.session = MNNAdapter(mnn_path, self.input_shape)

    def _run_session(self, data):
        outputs = self.session.inference(data)
        return outputs

    def _postprocess(self, data):
        assert data.shape[0] == 1
        data = np.asarray(data).reshape(-1, 4, 2)
        data[:, :, 0] *= self.input_size[1]
        data[:, :, 1] *= self.input_size[0]

        return data[0]

    def _preprocess(self, image):
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single " \
                               "image. "
        image_resize = cv2.resize(image, self.input_size)
        encode = encode_images(image_resize)
        encode = encode.astype(np.float32)
        input_tensor = np.expand_dims(encode, 0)

        return input_tensor


class BVTVertexORT(HamburgerABC):

    def __init__(self, onnx_path, *args, **kwargs):
        import onnxruntime as ort
        super().__init__(*args, **kwargs)
        self.session = ort.InferenceSession(onnx_path, None)
        self.input_config = self.session.get_inputs()[0]
        self.output_config = self.session.get_outputs()[0]
        self.input_size = self.input_config.shape[2:]

    # @cost('Vertex')
    def _run_session(self, data) -> np.ndarray:
        result = self.session.run([self.output_config.name], {self.input_config.name: data})

        return result[0]

    def _postprocess(self, data) -> np.ndarray:
        assert data.shape[0] == 1
        data = np.asarray(data).reshape(-1, 4, 2)
        data[:, :, 0] *= self.input_size[1]
        data[:, :, 1] *= self.input_size[0]

        return data[0]

    def _preprocess(self, image) -> np.ndarray:
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single " \
                               "image. "
        image_resize = cv2.resize(image, self.input_size)
        encode = encode_images(image_resize)
        encode = encode.astype(np.float32)
        input_tensor = np.expand_dims(encode, 0)

        return input_tensor
