# -*- coding: utf-8 -*-
import hyperlpr3 as lpr3
import cv2
import urllib
import numpy as np
import re
import click
from loguru import logger

type_list = ["蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层"]


def is_http_url(s):
    regex = re.compile(
        r'^(?:http|ftp)s?://'  # http:// or https://
        r'(?:(?:[A-Z0-9](?:[A-Z0-9-]{0,61}[A-Z0-9])?\.)+(?:[A-Z]{2,6}\.?|[A-Z0-9-]{2,}\.?)|'
        r'localhost|'  # localhost...
        r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})'  # ...or ip
        r'(?::\d+)?'  # optional port
        r'(?:/?|[/?]\S+)$', re.IGNORECASE)

    if regex.match(s):
        return True
    else:
        return False


def url_to_image(url):
    try:
        resp = urllib.request.urlopen(url)
        image = np.asarray(bytearray(resp.read()), dtype="uint8")
        image = cv2.imdecode(image, cv2.IMREAD_COLOR)
    except Exception as err:
        return None

    return image


def get_image(path: str):
    image = None
    if is_http_url(path):
        # url
        image = url_to_image(path)
    else:
        # local path
        if path.split('.')[-1].lower() in ('jpg', 'png', 'jpeg', 'bmp',):
            image = cv2.imread(path)
    try:
        h, w, c = image.shape
    except Exception as err:
        logger.error("Failed to read image from path or url.")
        return False, None

    return True, image


@click.command(help="Exec HyperLPR3 Test Sample.")
@click.option("-src", "--src", type=str, )
@click.option("-det", "--det", default='low', type=click.Choice(['low', 'high']), )
def sample(src, det):
    ret, image = get_image(src)
    if ret:
        if det == 'low':
            level = lpr3.DETECT_LEVEL_LOW
        else:
            level = lpr3.DETECT_LEVEL_HIGH
        catcher = lpr3.LicensePlateCatcher(detect_level=level)
        print("--" * 20)
        result = catcher(image)
        logger.info(f"共检测到车牌: {len(result)}")
        for res in result:
            code, conf, plate_type, box = res
            logger.success(f'[{type_list[plate_type]}]{code} {conf} {box}')


if __name__ == "__main__":
    sample()
