#coding=utf-8

from wxpy import *
import numpy
import cv2
import time
import os
from hyperlpr import pipline

def recognize(filename):
    image = cv2.imread(filename)
    return pipline.RecognizePlateJson(image)

bot = Bot(console_qr=True, cache_path=True)

@bot.register(Friend,PICTURE)
def pr_msg(msg):
    image_name = msg.file_name
    friend = msg.chat
    print msg.chat
    print '接收图片'
    # face(image_name)
    msg.get_file('' + msg.file_name)
    json_text = recognize(image_name)
    msg.reply(json_text)
    msg.reply_image("0.jpg")
    os.remove(image_name)
embed()
