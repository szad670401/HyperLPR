import numpy as np
import cv2
import time
from functools import wraps

def resize_bounding_box(bbox, scale_factor):
    """
    Resize a bounding box by a given scale factor, keeping the center point fixed.

    Args:
        bbox (list or tuple): The bounding box defined by [x1, y1, x2, y2].
        scale_factor (float): The factor by which to scale the bounding box.

    Returns:
        list: The resized bounding box defined by [x1, y1, x2, y2].
    """
    x1, y1, x2, y2 = bbox
    center_x = (x1 + x2) / 2
    center_y = (y1 + y2) / 2
    width = (x2 - x1) * scale_factor
    height = (y2 - y1) * scale_factor

    new_x1 = center_x - width / 2
    new_y1 = center_y - height / 2
    new_x2 = center_x + width / 2
    new_y2 = center_y + height / 2

    return [int(new_x1), int(new_y1), int(new_x2), int(new_y2)]

def plate_split(img):
    h, w, _ = img.shape
    line = int(h * 0.4)
    top = img[:line, :, ]
    bottom = img[line:, :]
    return top, bottom

def plate_squeeze(img):
    height, width, channels = img.shape

    upper_bound = int(0.416 * height)
    lower_bound = int(0.333 * height)

    upper_part = img[:upper_bound, :]
    lower_part = img[lower_bound:, :]

    upper_part_resized = cv2.resize(upper_part, (width, lower_part.shape[0]))

    merged_img = np.hstack((upper_part_resized, lower_part))

    return merged_img


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
