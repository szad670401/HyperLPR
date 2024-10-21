from hyperlpr3.algo.base.recognition import BaseTextRecognizer, TextRecognitionResult
import cv2
import numpy as np
import math

_words_token = ["blank", "'", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H",
                "J",
                "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "云", "京", "冀", "吉",
                "学", "宁",
                "川", "挂", "新", "晋", "桂", "民", "沪", "津", "浙", "渝", "港", "湘", "琼", "甘", "皖", "粤", "航",
                "苏", "蒙", "藏", "警", "豫",
                "贵", "赣", "辽", "鄂", "闽", "陕", "青", "鲁", "黑", '领', '使', '澳', ]


def get_ignored_tokens():
    return [0]  # for ctc blank


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


class CRNNCTCRecognizer(BaseTextRecognizer):

    def __init__(self, onnx_path, token_dict=_words_token, providers=['CPUExecutionProvider'], *args, **kwargs):
        """
        Initialize the CRNNCTCRecognizer class.

        :param onnx_path: Path to the ONNX model.
        :param token_dict: Dictionary of tokens for decoding.
        :param args: Additional positional arguments.
        :param kwargs: Additional keyword arguments.
        """
        self.onnx_path = onnx_path
        self.token_dict = token_dict
        self.providers = providers
        super().__init__(*args, **kwargs)

    def _load_model(self):
        import onnxruntime as ort
        """Load the ONNX model."""
        self.session = ort.InferenceSession(self.onnx_path, providers=self.providers)
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
        return result

    def _postprocess(self, data):
        """
        Postprocess the model output.

        :param data: Model output data.
        :return: Tuple containing the recognized text and average confidence.
        """
        if data:
            prod = data[0]
            argmax = np.argmax(prod, axis=2)
            rmax = np.max(prod, axis=2)
            result = self.decode(argmax, rmax, is_remove_duplicate=True)
            return result[0]
        else:
            return '', 0.0

    def _preprocess(self, image):
        """
        Preprocess the input image.

        :param image: Input image.
        :return: Preprocessed image.
        """
        assert len(
            image.shape) == 3, "The dimensions of the input image object do not match. The input supports a single image."
        h, w, _ = image.shape
        wh_ratio = w * 1.0 / h
        data = encode_images(image, wh_ratio, self.input_size)
        data = np.expand_dims(data, 0)
        return data

    def _normalized_output(self, data):
        """
        Standardize the final text recognition result output.

        :param data: The postprocessed recognition result.
        :return: A TextRecognitionResult instance containing the recognized text, average confidence, and character confidences.
        """
        text, avg_confidence = data
        char_confidences = [avg_confidence] * len(text)  # Assuming uniform confidence for simplicity
        return TextRecognitionResult(text, avg_confidence, char_confidences)

    def decode(self, text_index, text_prob=None, is_remove_duplicate=False):
        """
        Convert text-index into text-label.

        :param text_index: Indexes of the recognized text.
        :param text_prob: Probabilities of the recognized text.
        :param is_remove_duplicate: Whether to remove duplicate characters.
        :return: List of tuples containing the recognized text and average confidence.
        """
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
                    if idx > 0 and text_index[batch_idx][idx - 1] == text_index[batch_idx][idx]:
                        continue
                char_list.append(self.token_dict[int(text_index[batch_idx][idx])])
                if text_prob is not None:
                    conf_list.append(text_prob[batch_idx][idx])
                else:
                    conf_list.append(1)
            text = ''.join(char_list)
            result_list.append((text, np.mean(conf_list)))

        return result_list

