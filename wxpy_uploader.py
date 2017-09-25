#coding=utf-8

from wxpy import *
import numpy
import cv2
import time
import os
from hyperlpr import pipline

def recognize(filename):
    image = cv2.imread(filename)
    #通过文件名读入一张图片 放到 image中
    return pipline.RecognizePlateJson(image)
    #识别一张图片并返回json结果

# 人脸检测的功能，这里用到了OpenCV里面的人脸检测代码
# 由于接收和处理图片都需要一点点时间 这里偷懒直接用sleep
# 建议更改成多进程模式，否则容易造成进程混乱

bot = Bot(console_qr=True, cache_path=True)

# friend = bot.friends().search(u'庾金科')[0]
# friend.send(u'你好，车牌识别功能已开启测试')

@bot.register(Friend,PICTURE)
def face_msg(msg):
    image_name = msg.file_name
    friend = msg.chat
    print msg.chat
    print '接收图片'
    # face(image_name)
    msg.get_file('' + msg.file_name)
    # time.sleep(1)
    json_text = recognize(image_name)
    # face(image_name)
    # time.sleep(0.5)
    msg.reply(json_text)
    msg.reply_image("0.jpg")


    # msg.reply(json_text)


    os.remove(image_name)
embed()