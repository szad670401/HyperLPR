# -- coding: UTF-8

import cv2
import os
import hyperlpr.colourDetection as hc
import hyperlpr.config as hconfig

filepath = hconfig.configuration["colorTest"]["colorPath"]
for filename in os.listdir(filepath):
    if filename.endswith(".jpg") or filename.endswith(".png") or filename.endswith(".bmp"):
        fileFullPath = os.path.join(filepath,filename)
        img = cv2.imread(fileFullPath.encode('utf-8'))
        color,rgb = hc.judge_plate_color(img)
        if color != None:
            print filename,"->",color,"->",rgb
        else:
            print filename,"->","unknown->",rgb