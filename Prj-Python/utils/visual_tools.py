import cv2
import numpy as np
from PIL import Image, ImageDraw, ImageFont


def cv2ImgAddText(img, text, left, top, textColor=(255, 0, 0), textSize=20):
    if (isinstance(img, np.ndarray)):  # 判断是否OpenCV图片类型
        img = Image.fromarray(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
    draw = ImageDraw.Draw(img)
    fontText = ImageFont.truetype("resource/font/platech.ttf", textSize, encoding="utf-8")
    draw.text((left, top), text, textColor, font=fontText)

    return cv2.cvtColor(np.asarray(img), cv2.COLOR_RGB2BGR)


def draw_full(image: np.ndarray, plate_list: dict, ) -> np.ndarray:
    canvas = image.copy()
    for plate_info in plate_list:
        kps = plate_info['vertex']
        bdbox = plate_info['det_bound_box']
        text = plate_info['plate_code']
        x1, y1, x2, y2 = bdbox.astype(int)
        bd_y = y2 - y1
        bd_w = x2 - x1
        point_size = bd_w // 20
        line_size = bd_w // 40
        cv2.polylines(canvas, [kps.astype(np.int32)], True, (0, 0, 200), line_size, )
        for x, y in kps.astype(np.int32):
            cv2.line(canvas, (x, y), (x, y), (80, 240, 100), point_size)
        text_x = kps[0][0]
        text_y = kps[0][1] - bd_y // 1.4
        canvas = cv2ImgAddText(canvas, text, text_x, text_y, textSize=bd_w // 5)


    return canvas
