import numpy as np
import cv2
import time
from functools import wraps


def find_the_adjacent_boxes(boxes: list):
    list_index = list()
    for i, a in enumerate(boxes):
        for j, b in enumerate(boxes):
            if i == j:
                continue
            if j in list_index:
                continue
            ax, ay, aw, ah = single_xyxy2cxcywh(a)
            bx, by, bw, bh = single_xyxy2cxcywh(b)
            dis = l2((ax, ay), (bx, by))
            if dis < 2 * aw or dis < 2 * bw:
                list_index.append(i)
                list_index.append(j)

    list_index = set(list_index)

    return list(list_index)



def l2(p1, p2):
    x0, y0 = p1
    x1, y1 = p2
    return np.sqrt((x0 - x1) ** 2 + (y0 - y1) ** 2)


def single_xyxy2cxcywh(box):
    x1, y1, x2, y2 = box
    w = x2 - x1
    h = y2 - y1
    x = x1 + w / 2
    y = y1 + h / 2
    return x, y, w, h


def sigmoid(x):
    return 1 / (1 + np.exp(-x))


def xywh2xyxy(x):
    # Convert [x, y, w, h] to [x1, y1, x2, y2]
    y = np.copy(x)
    y[:, 0] = x[:, 0] - x[:, 2] / 2  # top left x
    y[:, 1] = x[:, 1] - x[:, 3] / 2  # top left y
    y[:, 2] = x[:, 0] + x[:, 2] / 2  # bottom right x
    y[:, 3] = x[:, 1] + x[:, 3] / 2  # bottom right y
    return y


def process(input, mask, anchors, size):
    anchors = [anchors[i] for i in mask]
    grid_h, grid_w = map(int, input.shape[0:2])

    box_confidence = sigmoid(input[..., 4])
    box_confidence = np.expand_dims(box_confidence, axis=-1)

    box_class_probs = sigmoid(input[..., 5:])

    box_xy = sigmoid(input[..., :2]) * 2 - 0.5

    col = np.tile(np.arange(0, grid_w), grid_h).reshape(-1, grid_w)
    row = np.tile(np.arange(0, grid_h).reshape(-1, 1), grid_w)
    col = col.reshape(grid_h, grid_w, 1, 1).repeat(3, axis=-2)
    row = row.reshape(grid_h, grid_w, 1, 1).repeat(3, axis=-2)
    grid = np.concatenate((col, row), axis=-1)
    box_xy += grid
    box_xy *= (int(size[1] / grid_h), int(size[0] / grid_w))

    box_wh = pow(sigmoid(input[..., 2:4]) * 2, 2)
    box_wh = box_wh * anchors

    box = np.concatenate((box_xy, box_wh), axis=-1)

    return box, box_confidence, box_class_probs


def filter_boxes(boxes, box_confidences, box_class_probs, box_threshold, nms_threshold):
    """Filter boxes with box threshold. It's a bit different with origin yolov5 post process!
    # Arguments
        boxes: ndarray, boxes of objects.
        box_confidences: ndarray, confidences of objects.
        box_class_probs: ndarray, class_probs of objects.
    # Returns
        boxes: ndarray, filtered boxes.
        classes: ndarray, classes for boxes.
        scores: ndarray, scores for boxes.
    """
    boxes = boxes.reshape(-1, 4)
    box_confidences = box_confidences.reshape(-1)
    box_class_probs = box_class_probs.reshape(-1, box_class_probs.shape[-1])

    _box_pos = np.where(box_confidences >= box_threshold)
    boxes = boxes[_box_pos]
    box_confidences = box_confidences[_box_pos]
    box_class_probs = box_class_probs[_box_pos]

    class_max_score = np.max(box_class_probs, axis=-1)
    classes = np.argmax(box_class_probs, axis=-1)
    _class_pos = np.where(class_max_score * box_confidences >= box_threshold)

    boxes = boxes[_class_pos]
    classes = classes[_class_pos]
    scores = (class_max_score * box_confidences)[_class_pos]

    return boxes, classes, scores


def nms_boxes(boxes, scores, nms_threshold):
    """Suppress non-maximal boxes.
    # Arguments
        boxes: ndarray, boxes of objects.
        scores: ndarray, scores of objects.
    # Returns
        keep: ndarray, index of effective boxes.
    """
    x = boxes[:, 0]
    y = boxes[:, 1]
    w = boxes[:, 2] - boxes[:, 0]
    h = boxes[:, 3] - boxes[:, 1]

    areas = w * h
    order = scores.argsort()[::-1]

    keep = []
    while order.size > 0:
        i = order[0]
        keep.append(i)

        xx1 = np.maximum(x[i], x[order[1:]])
        yy1 = np.maximum(y[i], y[order[1:]])
        xx2 = np.minimum(x[i] + w[i], x[order[1:]] + w[order[1:]])
        yy2 = np.minimum(y[i] + h[i], y[order[1:]] + h[order[1:]])

        w1 = np.maximum(0.0, xx2 - xx1 + 0.00001)
        h1 = np.maximum(0.0, yy2 - yy1 + 0.00001)
        inter = w1 * h1

        ovr = inter / (areas[i] + areas[order[1:]] - inter)
        inds = np.where(ovr <= nms_threshold)[0]
        order = order[inds + 1]
    keep = np.array(keep)
    return keep


def restore_bound_box(boxes: list, ratio: tuple, pad_size: tuple):
    if len(boxes) > 0:
        pad_width, pad_height = pad_size
        boxes_array = np.asarray(boxes)
        # print(boxes_array)
        boxes_array[:, 0] = (boxes_array[:, 0] - pad_width) / ratio[0]
        boxes_array[:, 1] = (boxes_array[:, 1] - pad_height) / ratio[1]
        boxes_array[:, 2] = (boxes_array[:, 2] - pad_width) / ratio[0]
        boxes_array[:, 3] = (boxes_array[:, 3] - pad_height) / ratio[1]
        # print(boxes_array)
        boxes = boxes_array.tolist()

    return boxes


def letterbox(im, new_shape=(640, 640), color=(0, 0, 0)):
    # Resize and pad image while meeting stride-multiple constraints
    shape = im.shape[:2]  # current shape [height, width]
    if isinstance(new_shape, int):
        new_shape = (new_shape, new_shape)

    # Scale ratio (new / old)
    r = min(new_shape[0] / shape[0], new_shape[1] / shape[1])

    # Compute padding
    ratio = r, r  # width, height ratios
    new_unpad = int(round(shape[1] * r)), int(round(shape[0] * r))
    dw, dh = new_shape[1] - new_unpad[0], new_shape[0] - new_unpad[1]  # wh padding

    dw /= 2  # divide padding into 2 sides
    dh /= 2

    if shape[::-1] != new_unpad:  # resize
        im = cv2.resize(im, new_unpad, interpolation=cv2.INTER_LINEAR)
    top, bottom = int(round(dh - 0.1)), int(round(dh + 0.1))
    left, right = int(round(dw - 0.1)), int(round(dw + 0.1))
    im = cv2.copyMakeBorder(im, top, bottom, left, right, cv2.BORDER_CONSTANT, value=color)  # add border
    return im, ratio, (dw, dh)


def cost(tag=''):
    try:
        '''
        :param tag: 装饰器的参数
        '''
        from loguru import logger
        def wrapper(fn):
            @wraps(fn)
            def wrapper_use_time(*args, **kw):
                '''
                函数传参方式 fn(arg1, arg2, param1=a, param2=b)
                :param args: 位置参数(arg1, arg2, ....)  传参方式 fn(arg1, arg2, ...)
                :param kw: 字典参数{param1=a, param2=b, .....} 传参方式 fn(param1=a, param2=b, ....)
                :return:
                '''
                t1 = time.time()
                try:
                    res = fn(*args, **kw)
                except Exception as e:
                    logger.error(f"@use_time %s(%s) execute error" % (fn.__name__, tag))
                    return None
                else:
                    t2 = time.time()
                    logger.info(f"{tag}@UseTime: {t2 - t1}")
                    return res

            return wrapper_use_time

        return wrapper
    except Exception as err:
        print(err)



def align_box(imgs, bbox, size=96, scale_factor=1.0, center_bias=0, borderValue=(0, 0, 0)):
    bias_x = (-1 + 2 * np.random.sample()) * center_bias
    bias_y = (-1 + 2 * np.random.sample()) * center_bias
    b_x1, b_y1, b_x2, b_y2 = bbox
    cx, cy = (b_x1 + b_x2) // 2, (b_y1 + b_y2) // 2
    w = b_x2 - b_x1
    h = b_y2 - b_y1
    cx += w * bias_x
    cy += h * bias_y

    base_r = max(w, h)
    j_x = 0
    j_y = 0
    j_r = 0
    base_r += j_r
    r = int(base_r / 2 * scale_factor)
    cy -= int(base_r * 0)
    cx += j_x
    cy += j_y
    x1, y1, x2, y2 = cx - r, cy - r, cx + r, cy + r
    x3, y3 = cx - r, cy + r
    _x1, _y1, _x2, _y2, _x3, _y3 = [0, 0, size, size, 0, size]
    src = np.array([x1, y1, x2, y2, x3, y3], dtype=np.float32).reshape(3, 2)
    sv = np.asarray([[b_x1, b_y1, 1], [b_x2, b_y2, 1]])

    dst = np.array([_x1, _y1, _x2, _y2, _x3, _y3], dtype=np.float32).reshape(3, 2)
    assert src.dtype == np.float32
    assert dst.dtype == np.float32
    assert src.shape == (3, 2)
    assert dst.shape == (3, 2)
    mat = cv2.getAffineTransform(src, dst)
    p = sv.dot(mat.T).reshape(-1)
    if type(imgs) == list:
        imgs = [cv2.warpAffine(img, mat, (size, size), borderValue=borderValue) for img in imgs]
    else:
        imgs = cv2.warpAffine(imgs, mat, (size, size), borderValue=borderValue)
    return imgs, p, mat


def get_rotate_crop_image(img, points):
    '''
    img_height, img_width = img.shape[0:2]
    left = int(np.min(points[:, 0]))
    right = int(np.max(points[:, 0]))
    top = int(np.min(points[:, 1]))
    bottom = int(np.max(points[:, 1]))
    img_crop = img[top:bottom, left:right, :].copy()
    points[:, 0] = points[:, 0] - left
    points[:, 1] = points[:, 1] - top
    '''
    assert len(points) == 4, "shape of points must be 4*2"
    img_crop_width = int(
        max(
            np.linalg.norm(points[0] - points[1]),
            np.linalg.norm(points[2] - points[3])))
    img_crop_height = int(
        max(
            np.linalg.norm(points[0] - points[3]),
            np.linalg.norm(points[1] - points[2])))
    pts_std = np.float32([[0, 0], [img_crop_width, 0],
                          [img_crop_width, img_crop_height],
                          [0, img_crop_height]])
    points = points.astype(np.float32)
    # print(points.shape, pts_std.shape)
    M = cv2.getPerspectiveTransform(points, pts_std)
    dst_img = cv2.warpPerspective(
        img,
        M, (img_crop_width, img_crop_height),
        borderMode=cv2.BORDER_REPLICATE,
        flags=cv2.INTER_CUBIC)
    dst_img_height, dst_img_width = dst_img.shape[0:2]
    if dst_img_height * 1.0 / dst_img_width >= 1.5:
        dst_img = np.rot90(dst_img)

    return dst_img
