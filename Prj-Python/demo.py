# 导入cv相关库
import cv2
import numpy as np
from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw
# 导入依赖包
import hyperlpr3 as lpr3


def draw_plate_on_image(img, box, text, font):
    x1, y1, x2, y2 = box
    cv2.rectangle(img, (x1, y1), (x2, y2), (139, 139, 102), 2, cv2.LINE_AA)
    cv2.rectangle(img, (x1, y1 - 20), (x2, y1), (139, 139, 102), -1)
    data = Image.fromarray(img)
    draw = ImageDraw.Draw(data)
    draw.text((x1 + 5, y1 - 20), text, (255, 255, 255), font=font)
    res = np.asarray(data)

    return res


# 中文字体加载
font_ch = ImageFont.truetype("../resource/font/platech.ttf", 20, 0)

# 实例化识别对象
catcher = lpr3.LicensePlateCatcher(detect_level=lpr3.DETECT_LEVEL_HIGH)
# 读取图片
image = cv2.imread("/Users/tunm/Downloads/tlj.webp")

# 执行识别算法
results = catcher(image)
for code, confidence, type_idx, box in results:
    # 解析数据并绘制
    text = f"{code} - {confidence:.2f}"
    image = draw_plate_on_image(image, box, text, font=font_ch)

# 显示检测结果
cv2.imshow("w", image)
cv2.waitKey(0)