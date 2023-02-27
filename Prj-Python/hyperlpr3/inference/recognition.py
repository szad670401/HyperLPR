import cv2
import numpy as np
from .base.base import HamburgerABC
from hyperlpr3.common.tools_process import cost
import math
from hyperlpr3.common.tokenize import token


def encode_images(image: np.ndarray, max_wh_ratio, target_shape, limited_max_width=160, limited_min_width=48):
    imgC = 3
    imgH, imgW = target_shape
    # cv2.imshow("image", image)
    # cv2.waitKey(0)
    assert imgC == image.shape[2]
    max_wh_ratio = max(max_wh_ratio, imgW / imgH)
    imgW = int((imgH * max_wh_ratio))
    imgW = max(min(imgW, limited_max_width), limited_min_width)
    h, w = image.shape[:2]
    ratio = w / float(h)
    ratio_imgH = math.ceil(imgH * ratio)
    ratio_imgH = max(ratio_imgH, limited_min_width)
    if ratio_imgH > imgW:
        resized_w = imgW
    else:
        resized_w = int(ratio_imgH)
    resized_image = cv2.resize(image, (resized_w, imgH))
    # print((resized_w, imgH))
    # padding_im1 = np.ones((imgH, imgW, imgC), dtype=np.uint8) * 128
    # padding_im1[:, 0:resized_w, :] = resized_image
    # cv2.imwrite("pad.jpg", padding_im1)

    resized_image = resized_image.astype('float32')
    resized_image = (resized_image.transpose((2, 0, 1)) - 127.5) / 127.5
    # resized_image -= 0.5
    # resized_image *= 2
    padding_im = np.zeros((imgC, imgH, imgW), dtype=np.float32)
    padding_im[:, :, 0:resized_w] = resized_image

    # np.save('fk.npy', padding_im)

    return padding_im


def get_ignored_tokens():
    return [0]  # for ctc blank


class PPRCNNRecognitionMNN(HamburgerABC):

    def __init__(self, mnn_path, character_file, *args, **kwargs):
        from hyperlpr3.common.mnn_adapt import MNNAdapter
        super().__init__(*args, **kwargs)
        self.input_shape = (1, 3, self.input_size[0], self.input_size[1])
        self.session = MNNAdapter(mnn_path, input_shape=self.input_shape, outputs_name=['output'])
        self.character_list = token

    def decode(self, text_index, text_prob=None, is_remove_duplicate=False):
        """ convert text-index into text-label. """
        result_list = []
        ignored_tokens = get_ignored_tokens()
        batch_size = len(text_index)
        for batch_idx in range(batch_size):
            char_list = []
            conf_list = []
            for idx in range(len(text_index[batch_idx])):
                if text_index[batch_idx][idx] in ignored_tokens:
                    continue
                if is_remove_duplicate:
                    # only for predict
                    if idx > 0 and text_index[batch_idx][idx - 1] == text_index[batch_idx][idx]:
                        continue
                char_list.append(self.character_list[int(text_index[batch_idx][idx])])
                if text_prob is not None:
                    conf_list.append(text_prob[batch_idx][idx])
                else:
                    conf_list.append(1)
            text = ''.join(char_list)
            result_list.append((text, np.mean(conf_list)))
        return result_list

    def _run_session(self, data):
        output = self.session.inference(data)
        output = output.reshape(40, 6625)
        # print(output[:, 0])
        output = np.expand_dims([output], 0)
        return output

    def _postprocess(self, data):
        prod = data[0]
        argmax = np.argmax(prod, axis=2)
        # print(argmax)
        rmax = np.max(prod, axis=2)
        # print(rmax)
        result = self.decode(argmax, rmax, is_remove_duplicate=True)

        return result[0]

    def _preprocess(self, image):
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single " \
                               "image. "
        h, w, _ = image.shape
        wh_ratio = w * 1.0 / h
        data = encode_images(image, wh_ratio, self.input_size, )
        data = np.expand_dims(data, 0)

        return data


class PPRCNNRecognitionORT(HamburgerABC):

    def __init__(self, onnx_path, token_dict=token, *args, **kwargs):
        import onnxruntime as ort
        super().__init__(*args, **kwargs)
        self.session = ort.InferenceSession(onnx_path, None)
        self.input_config = self.session.get_inputs()[0]
        self.output_config = self.session.get_outputs()[0]
        self.input_size = self.input_config.shape[2:]
        # print(self.input_size)
        self.character_list = token_dict

    def decode(self, text_index, text_prob=None, is_remove_duplicate=False):
        """ convert text-index into text-label. """
        result_list = []
        ignored_tokens = get_ignored_tokens()
        batch_size = len(text_index)
        for batch_idx in range(batch_size):
            char_list = []
            conf_list = []
            for idx in range(len(text_index[batch_idx])):
                if text_index[batch_idx][idx] in ignored_tokens:
                    continue
                if is_remove_duplicate:
                    # only for predict
                    if idx > 0 and text_index[batch_idx][idx - 1] == text_index[batch_idx][idx]:
                        continue
                # print(int(text_index[batch_idx][idx]))
                char_list.append(self.character_list[int(text_index[batch_idx][idx])])
                if text_prob is not None:
                    conf_list.append(text_prob[batch_idx][idx])
                else:
                    conf_list.append(1)
            text = ''.join(char_list)
            result_list.append((text, np.mean(conf_list)))
        return result_list

    # @cost("Recognition")
    def _run_session(self, data) -> np.ndarray:
        result = self.session.run([self.output_config.name], {self.input_config.name: data})

        return result

    def _postprocess(self, data) -> tuple:
        if data:
            prod = data[0]
            argmax = np.argmax(prod, axis=2)
            rmax = np.max(prod, axis=2)
            result = self.decode(argmax, rmax, is_remove_duplicate=True)

            return result[0]
        else:
            return '', 0.0

    def _preprocess(self, image) -> np.ndarray:
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single " \
                               "image. "
        h, w, _ = image.shape
        wh_ratio = w * 1.0 / h
        data = encode_images(image, wh_ratio, self.input_size, )
        data = np.expand_dims(data, 0)
        # print(data.shape)

        return data


class PPRCNNRecognitionDNN(HamburgerABC):

    def __init__(self, onnx_path, character_file, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.session = cv2.dnn.readNetFromONNX(onnx_path)
        self.input_shape = (1, 3, self.input_size[0], self.input_size[1])
        self.character_list = token

    def decode(self, text_index, text_prob=None, is_remove_duplicate=False):
        result_list = []
        ignored_tokens = get_ignored_tokens()
        batch_size = len(text_index)
        for batch_idx in range(batch_size):
            char_list = []
            conf_list = []
            for idx in range(len(text_index[batch_idx])):
                if text_index[batch_idx][idx] in ignored_tokens:
                    continue
                if is_remove_duplicate:
                    # only for predict
                    if idx > 0 and text_index[batch_idx][idx - 1] == text_index[batch_idx][idx]:
                        continue
                char_list.append(self.character_list[int(text_index[batch_idx][idx])])
                if text_prob is not None:
                    conf_list.append(text_prob[batch_idx][idx])
                else:
                    conf_list.append(1)
            text = ''.join(char_list)
            result_list.append((text, np.mean(conf_list)))
        return result_list

    def _run_session(self, data):
        self.session.setInput(data)
        outputs = self.session.forward()
        outputs = np.expand_dims(outputs, 0)
        # print(outputs.shape)

        return outputs

    def _postprocess(self, data):
        prod = data[0]
        argmax = np.argmax(prod, axis=2)
        rmax = np.max(prod, axis=2)
        result = self.decode(argmax, rmax, is_remove_duplicate=True)

        return result[0]

    def _preprocess(self, image):
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single " \
                               "image. "
        h, w, _ = image.shape
        wh_ratio = w * 1.0 / h
        data = encode_images(image, wh_ratio, self.input_size, )
        data = np.expand_dims(data, 0)

        return data
