from hyperlpr3.common.tools_process import *
from .base.base import HamburgerABC

ANCHORS_MAP = {
    320: [[9.38281, 3.08398], [15.53125, 4.93750], [19.98438, 7.78906],
          [31.10938, 10.35156], [45.21875, 14.14844], [32.34375, 21.04688],
          [65.62500, 19.57812], [76.12500, 46.12500], [253.25000, 137.50000]],
    640: [[10, 13], [16, 30], [33, 23], [30, 61], [62, 45],
          [59, 119], [116, 90], [156, 198], [373, 326]]
}


def image_to_input_tensor(image):
    data = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    data = data.transpose(2, 0, 1) / 255.0
    data = np.expand_dims(data, 0)
    data = data.astype(np.float32)

    return data


class Y5rkDetectorMNN(HamburgerABC):
    def __init__(self, mnn_path, box_threshold: float = 0.5, nms_threshold: float = 0.6, *args, **kwargs):
        from .common.mnn_adapt import MNNAdapter
        super().__init__(*args, **kwargs)
        self.box_threshold = box_threshold
        self.nms_threshold = nms_threshold
        self.input_shape = (1, 3, self.input_size[0], self.input_size[1])
        self.tensor_shape = ((1, 18, 40, 40), (1, 18, 20, 20), (1, 18, 10, 10))
        self.session = MNNAdapter(mnn_path, self.input_shape, outputs_name=['output', '335', '336'],
                                  outputs_shape=self.tensor_shape)
        assert self.input_size[0] == self.input_size[1]
        self.anchors = ANCHORS_MAP[self.input_size[0]]

    def _run_session(self, data):
        outputs = self.session.inference(data)
        result = list()
        for idx, output in enumerate(outputs):
            result.append(output.reshape(self.tensor_shape[idx]))

        return result

    def _postprocess(self, data):
        ratio, (dw, dh) = self.temp_pack
        input0_data = data[0]
        input1_data = data[1]
        input2_data = data[2]
        input0_data = input0_data.reshape([3, -1] + list(input0_data.shape[-2:]))
        input1_data = input1_data.reshape([3, -1] + list(input1_data.shape[-2:]))
        input2_data = input2_data.reshape([3, -1] + list(input2_data.shape[-2:]))
        input_data = list()
        input_data.append(np.transpose(input0_data, (2, 3, 0, 1)))
        input_data.append(np.transpose(input1_data, (2, 3, 0, 1)))
        input_data.append(np.transpose(input2_data, (2, 3, 0, 1)))

        boxes, classes, scores = self.decode_outputs(input_data, self.input_size)

        boxes = restore_bound_box(boxes, ratio, (dw, dh))

        return boxes, classes, scores

    def _preprocess(self, image):
        h, w, _ = image.shape
        resize_img, ratio, (dw, dh) = letterbox(image, new_shape=(self.input_size[1], self.input_size[0]))
        data = image_to_input_tensor(resize_img)
        self.temp_pack = ratio, (dw, dh)

        return data

    def decode_outputs(self, input_data, size):
        masks = [[0, 1, 2], [3, 4, 5], [6, 7, 8]]
        anchors = self.anchors

        boxes, classes, scores = [], [], []
        for input, mask in zip(input_data, masks):
            b, c, s = process(input, mask, anchors, size)
            b, c, s = filter_boxes(b, c, s, self.box_threshold, self.nms_threshold)
            boxes.append(b)
            classes.append(c)
            scores.append(s)

        boxes = np.concatenate(boxes)
        boxes = xywh2xyxy(boxes)
        classes = np.concatenate(classes)
        scores = np.concatenate(scores)

        nboxes, nclasses, nscores = [], [], []
        for c in set(classes):
            inds = np.where(classes == c)
            b = boxes[inds]
            c = classes[inds]
            s = scores[inds]

            keep = nms_boxes(b, s, self.nms_threshold)

            nboxes.append(b[keep])
            nclasses.append(c[keep])
            nscores.append(s[keep])

        if not nclasses and not nscores:
            return None, None, None

        boxes = np.concatenate(nboxes)
        classes = np.concatenate(nclasses)
        scores = np.concatenate(nscores)

        return boxes, classes, scores


class Y5rkDetectorORT(HamburgerABC):

    def __init__(self, onnx_path, box_threshold: float = 0.5, nms_threshold: float = 0.6, *args, **kwargs):
        import onnxruntime as ort
        super().__init__(*args, **kwargs)
        self.box_threshold = box_threshold
        self.nms_threshold = nms_threshold
        self.session = ort.InferenceSession(onnx_path, None)
        self.inputs_option = self.session.get_inputs()
        self.outputs_option = self.session.get_outputs()
        input_option = self.inputs_option[0]
        input_size_ = tuple(input_option.shape[2:])
        self.input_size = tuple(self.input_size)
        if not self.input_size:
            self.input_size = input_size_
        assert self.input_size == input_size_, 'The dimensions of the input do not match the model expectations.'
        assert self.input_size[0] == self.input_size[1]
        self.input_name = input_option.name
        self.anchors = ANCHORS_MAP[self.input_size[0]]

    def decode_outputs(self, input_data, size):
        masks = [[0, 1, 2], [3, 4, 5], [6, 7, 8]]
        anchors = self.anchors

        boxes, classes, scores = [], [], []
        for input, mask in zip(input_data, masks):
            b, c, s = process(input, mask, anchors, size)
            b, c, s = filter_boxes(b, c, s, self.box_threshold, self.nms_threshold)
            boxes.append(b)
            classes.append(c)
            scores.append(s)

        boxes = np.concatenate(boxes)
        boxes = xywh2xyxy(boxes)
        classes = np.concatenate(classes)
        scores = np.concatenate(scores)

        nboxes, nclasses, nscores = [], [], []
        for c in set(classes):
            inds = np.where(classes == c)
            b = boxes[inds]
            c = classes[inds]
            s = scores[inds]

            keep = nms_boxes(b, s, self.nms_threshold)

            nboxes.append(b[keep])
            nclasses.append(c[keep])
            nscores.append(s[keep])

        if not nclasses and not nscores:
            return None, None, None

        boxes = np.concatenate(nboxes)
        classes = np.concatenate(nclasses)
        scores = np.concatenate(nscores)

        return boxes, classes, scores

    @cost("Detect")
    def _run_session(self, data):
        outputs = self.session.run([], {"images": data})

        return outputs

    def _postprocess(self, data):
        ratio, (dw, dh) = self.temp_pack
        input0_data = data[0]
        input1_data = data[1]
        input2_data = data[2]
        input0_data = input0_data.reshape([3, -1] + list(input0_data.shape[-2:]))
        input1_data = input1_data.reshape([3, -1] + list(input1_data.shape[-2:]))
        input2_data = input2_data.reshape([3, -1] + list(input2_data.shape[-2:]))
        input_data = list()
        input_data.append(np.transpose(input0_data, (2, 3, 0, 1)))
        input_data.append(np.transpose(input1_data, (2, 3, 0, 1)))
        input_data.append(np.transpose(input2_data, (2, 3, 0, 1)))

        boxes, classes, scores = self.decode_outputs(input_data, self.input_size)

        boxes = restore_bound_box(boxes, ratio, (dw, dh))

        return boxes, classes, scores

    def _preprocess(self, image):
        h, w, _ = image.shape
        resize_img, ratio, (dw, dh) = letterbox(image, new_shape=(self.input_size[1], self.input_size[0]))
        data = image_to_input_tensor(resize_img)
        self.temp_pack = ratio, (dw, dh)

        return data
