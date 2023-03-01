import numpy as np

from hyperlpr3.common.typedef import *
from hyperlpr3.common.tools_process import *


class LPRMultiTaskPipeline(object):

    def __init__(self, detector, recognizer, classifier, full_result=False):
        self.detector = detector
        self.recognizer = recognizer
        self.classifier = classifier
        self.full_result = full_result

    def run(self, image: np.ndarray) -> list:
        result = list()
        assert len(image.shape) == 3, "Input image must be 3 channels."
        assert image is not None, "Input image cannot be empty."
        outputs = self.detector(image)
        for out in outputs:
            rect = out[:4].astype(int)
            score = out[4]
            land_marks = out[5:13].reshape(4, 2).astype(int)
            layer_num = int(out[13])
            # print(layer_num)
            pad = get_rotate_crop_image(image, land_marks)
            if layer_num == DOUBLE:
                # double
                h, w, _ = pad.shape
                line = int(h * 0.4)
                top = pad[:line, :, ]
                bottom = pad[line:, :]
                top_code, top_confidence = self.recognizer(top)
                bottom_code, bottom_confidence = self.recognizer(bottom)
                plate_code = top_code + bottom_code
                rec_confidence = (top_confidence + bottom_confidence) / 2
                # cv2.imshow("top", top)
                # cv2.imshow("bottom", bottom)
                # cv2.waitKey(0)
            else:
                plate_code, rec_confidence = self.recognizer(pad)
            if plate_code == '':
                continue
            if len(plate_code) >= 7:
                plate_type = code_filter(plate_code)
                if plate_type == UNKNOWN:
                    cls = self.classifier(pad)
                    idx = int(np.argmax(cls))
                    if idx == PLATE_TYPE_YELLOW:
                        if layer_num == DOUBLE:
                            plate_type = YELLOW_DOUBLE
                        else:
                            plate_type = YELLOW_SINGLE
                    elif idx == PLATE_TYPE_BLUE:
                        plate_type = BLUE
                    elif idx == PLATE_TYPE_GREEN:
                        plate_type = GREEN
                plate = Plate(vertex=land_marks, plate_code=plate_code, det_bound_box=np.asarray(rect),
                              rec_confidence=rec_confidence, dex_bound_confidence=score, plate_type=plate_type)
                if self.full_result:
                    result.append(plate.to_full_result())
                else:
                    result.append(plate.to_result())

        return result

    def __call__(self, image: np.ndarray, *args, **kwargs):
        return self.run(image)


class LPRPipeline(object):

    def __init__(self, detector, vertex_predictor, recognizer, ):
        self.detector = detector
        self.vertex_predictor = vertex_predictor
        self.recognizer = recognizer

    # @cost("PipelineTotalCost")
    def run(self, image: np.ndarray) -> list:
        result = list()
        boxes, classes, scores = self.detector(image)
        fp_boxes_index = find_the_adjacent_boxes(boxes)
        image_blacks = list()
        if len(fp_boxes_index) > 0:
            for idx in fp_boxes_index:
                image_black = np.zeros_like(image)
                box = boxes[idx]
                x1, y1, x2, y2 = np.asarray(box).astype(int)
                image_black[y1:y2, x1:x2] = image[y1:y2, x1:x2]
                image_blacks.append(image_black)
        if boxes:
            fp = 0
            for idx, box in enumerate(boxes):
                det_confidence = scores[idx]
                if idx in fp_boxes_index:
                    warped, p, mat = align_box(image_blacks[fp], box, scale_factor=1.2, size=96)
                    fp += 1
                else:
                    warped, p, mat = align_box(image, box, scale_factor=1.2, size=96)
                kps = self.vertex_predictor(warped)
                polyline = list()
                for point in kps:
                    polyline.append([point[0], point[1], 1])
                polyline = np.asarray(polyline)
                inv = cv2.invertAffineTransform(mat)
                trans_points = np.dot(inv, polyline.T).T
                pad = get_rotate_crop_image(image, trans_points)
                # print(pad.shape)
                # cv2.imshow("pad", pad)
                # cv2.waitKey(0)
                plate_code, rec_confidence = self.recognizer(pad)
                if plate_code == '':
                    continue
                plate = Plate(vertex=trans_points, plate_code=plate_code, det_bound_box=np.asarray(box),
                              rec_confidence=rec_confidence, dex_bound_confidence=det_confidence)
                result.append(plate.to_dict())

        return result

    def __call__(self, image: np.ndarray, *args, **kwargs):
        return self.run(image)
