from hyperlpr3.algo.base.detection import BaseDetector, DetectionResult
import copy
import numpy as np
import cv2

def xywh2xyxy(boxes):
    """
    Convert bounding boxes from (center x, center y, width, height) format to (x1, y1, x2, y2) format.

    Args:
        boxes (np.ndarray): Array of bounding boxes in (center x, center y, width, height) format.

    Returns:
        np.ndarray: Array of bounding boxes in (x1, y1, x2, y2) format.
    """
    xywh = copy.deepcopy(boxes)
    xywh[:, 0] = boxes[:, 0] - boxes[:, 2] / 2
    xywh[:, 1] = boxes[:, 1] - boxes[:, 3] / 2
    xywh[:, 2] = boxes[:, 0] + boxes[:, 2] / 2
    xywh[:, 3] = boxes[:, 1] + boxes[:, 3] / 2
    return xywh

def nms(boxes, iou_thresh):
    """
    Perform Non-Maximum Suppression (NMS) on bounding boxes.

    Args:
        boxes (np.ndarray): Array of bounding boxes with scores.
        iou_thresh (float): Intersection over Union (IoU) threshold for suppression.

    Returns:
        list: Indices of the kept boxes after NMS.
    """
    index = np.argsort(boxes[:, 4])[::-1]
    keep = []
    while index.size > 0:
        i = index[0]
        keep.append(i)
        x1 = np.maximum(boxes[i, 0], boxes[index[1:], 0])
        y1 = np.maximum(boxes[i, 1], boxes[index[1:], 1])
        x2 = np.minimum(boxes[i, 2], boxes[index[1:], 2])
        y2 = np.minimum(boxes[i, 3], boxes[index[1:], 3])

        w = np.maximum(0, x2 - x1)
        h = np.maximum(0, y2 - y1)

        inter_area = w * h
        union_area = (boxes[i, 2] - boxes[i, 0]) * (boxes[i, 3] - boxes[i, 1]) + (
                boxes[index[1:], 2] - boxes[index[1:], 0]) * (boxes[index[1:], 3] - boxes[index[1:], 1])
        iou = inter_area / (union_area - inter_area)
        idx = np.where(iou <= iou_thresh)[0]
        index = index[idx + 1]
    return keep

def restore_box(boxes, r, left, top):
    """
    Restore bounding box coordinates to the original image scale.

    Args:
        boxes (np.ndarray): Array of bounding boxes.
        r (float): Scale ratio.
        left (int): Left padding.
        top (int): Top padding.

    Returns:
        np.ndarray: Array of restored bounding boxes.
    """
    boxes[:, [0, 2, 5, 7, 9, 11]] -= left
    boxes[:, [1, 3, 6, 8, 10, 12]] -= top

    boxes[:, [0, 2, 5, 7, 9, 11]] /= r
    boxes[:, [1, 3, 6, 8, 10, 12]] /= r
    return boxes

def detect_pre_precessing(img, img_size):
    """
    Preprocess the image for detection.

    Args:
        img (np.ndarray): Input image.
        img_size (tuple): Desired image size.

    Returns:
        tuple: Preprocessed image, scale ratio, left padding, and top padding.
    """
    img, r, left, top = letter_box(img, img_size)
    img = img[:, :, ::-1].transpose(2, 0, 1).copy().astype(np.float32)
    img = img / 255
    img = img.reshape(1, *img.shape)
    return img, r, left, top

def post_precessing(dets, r, left, top, conf_thresh=0.25, iou_thresh=0.5):
    """
    Postprocess the detection results.

    Args:
        dets (np.ndarray): Detection results.
        r (float): Scale ratio.
        left (int): Left padding.
        top (int): Top padding.
        conf_thresh (float): Confidence threshold.
        iou_thresh (float): IoU threshold for NMS.

    Returns:
        np.ndarray: Postprocessed detection results.
    """
    choice = dets[:, :, 4] > conf_thresh
    dets = dets[choice]
    dets[:, 13:15] *= dets[:, 4:5]
    box = dets[:, :4]
    boxes = xywh2xyxy(box)
    score = np.max(dets[:, 13:15], axis=-1, keepdims=True)
    index = np.argmax(dets[:, 13:15], axis=-1).reshape(-1, 1)
    output = np.concatenate((boxes, score, dets[:, 5:13], index), axis=1)
    reserve_ = nms(output, iou_thresh)
    output = output[reserve_]
    output = restore_box(output, r, left, top)
    return output

def letter_box(img, size=(640, 640)):
    """
    Resize and pad image to meet the desired size.

    Args:
        img (np.ndarray): Input image.
        size (tuple): Desired image size.

    Returns:
        tuple: Resized and padded image, scale ratio, left padding, and top padding.
    """
    h, w, c = img.shape
    r = min(size[0] / h, size[1] / w)
    new_h, new_w = int(h * r), int(w * r)
    top = int((size[0] - new_h) / 2)
    left = int((size[1] - new_w) / 2)

    bottom = size[0] - new_h - top
    right = size[1] - new_w - left
    img_resize = cv2.resize(img, (new_w, new_h))
    img = cv2.copyMakeBorder(img_resize, top, bottom, left, right, borderType=cv2.BORDER_CONSTANT, value=(0, 0, 0))
    return img, r, left, top

class YoloMultiTaskDetector(BaseDetector):

    def __init__(self, onnx_path, box_threshold: float = 0.5, nms_threshold: float = 0.6, providers=['CPUExecutionProvider'], *args, **kwargs):
        """
        Initialize the YOLO Multi-Task Detector.

        Args:
            onnx_path (str): Path to the ONNX model.
            box_threshold (float): Confidence threshold for bounding boxes.
            nms_threshold (float): IoU threshold for NMS.
            providers (list): List of providers for ONNX Runtime.
            *args: Additional positional arguments.
            **kwargs: Additional keyword arguments.
        """
        self.onnx_path = onnx_path
        self.box_threshold = box_threshold
        self.nms_threshold = nms_threshold
        self.tmp_pack = None
        self.providers = providers
        super().__init__(*args, **kwargs)

    def _load_model(self):
        """
        Load the ONNX model using ONNX Runtime.
        """
        import onnxruntime as ort
        self.session = ort.InferenceSession(self.onnx_path, providers=self.providers)
        self.inputs_option = self.session.get_inputs()
        self.outputs_option = self.session.get_outputs()
        input_option = self.inputs_option[0]
        input_size_ = tuple(input_option.shape[2:])
        if not self.input_size:
            self.input_size = input_size_
        assert self.input_size == input_size_, 'The dimensions of the input do not match the model expectations.'
        assert self.input_size[0] == self.input_size[1]
        self.input_name = input_option.name
        self.running = True

    def _run_session(self, data):
        """
        Run the ONNX model inference session.

        Args:
            data (np.ndarray): Preprocessed input data.

        Returns:
            np.ndarray: Model inference output.
        """
        result = self.session.run([self.outputs_option[0].name], {self.input_name: data})[0]
        return result

    def _postprocess(self, data):
        """
        Postprocess the model output.

        Args:
            data (np.ndarray): Model output data.

        Returns:
            np.ndarray: Postprocessed detection results.
        """
        r, left, top = self.tmp_pack
        return post_precessing(data, r, left, top)

    def _preprocess(self, image):
        """
        Preprocess the input image.

        Args:
            image (np.ndarray): Input image.

        Returns:
            np.ndarray: Preprocessed image.
        """
        img, r, left, top = detect_pre_precessing(image, self.input_size)
        self.tmp_pack = r, left, top
        return img

    def _normalized_output(self, data):
        """
        Standardize the final detection result output.

        Args:
            data (np.ndarray): The postprocessed detection result.

        Returns:
            list: A list of standardized detection results.
        """
        results = []
        for detection in data:
            x1, y1, x2, y2, confidence = detection[:5]
            keypoints = detection[5:13].reshape(-1, 2).tolist()
            layer = int(detection[13])  # Set layer from detection[13]
            results.append(DetectionResult(x1, y1, x2, y2, confidence, np.asarray(keypoints), layer))
        return results