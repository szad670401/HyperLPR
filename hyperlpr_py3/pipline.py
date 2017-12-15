#coding=utf-8
from . import detect
from . import  finemapping  as  fm

from . import segmentation
import cv2

import time
import numpy as np

from PIL import ImageFont
from PIL import Image
from PIL import ImageDraw
import json

import sys
from . import typeDistinguish as td
import imp


imp.reload(sys)
fontC = ImageFont.truetype("./Font/platech.ttf", 14, 0);

from . import e2e
#寻找车牌左右边界

def find_edge(image):
    sum_i = image.sum(axis=0)
    sum_i =  sum_i.astype(np.float)
    sum_i/=image.shape[0]*255
    # print sum_i

    start= 0 ;
    end = image.shape[1]-1

    for i,one in enumerate(sum_i):
        if one>0.4:
            start = i;
            if start-3<0:
                start = 0
            else:
                start -=3

            break;
    for i,one in enumerate(sum_i[::-1]):

        if one>0.4:
            end = end - i;
            if end+4>image.shape[1]-1:
                end = image.shape[1]-1
            else:
                end+=4
            break
    return start,end


#垂直边缘检测
def verticalEdgeDetection(image):
    image_sobel = cv2.Sobel(image.copy(),cv2.CV_8U,1,0)
    # image = auto_canny(image_sobel)

    # img_sobel, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT
    # canny_image  = auto_canny(image)
    flag,thres = cv2.threshold(image_sobel,0,255,cv2.THRESH_OTSU|cv2.THRESH_BINARY)
    print(flag)
    flag,thres = cv2.threshold(image_sobel,int(flag*0.7),255,cv2.THRESH_BINARY)
    # thres = simpleThres(image_sobel)
    kernal = np.ones(shape=(3,15))
    thres = cv2.morphologyEx(thres,cv2.MORPH_CLOSE,kernal)
    return thres


#确定粗略的左右边界
def horizontalSegmentation(image):

    thres = verticalEdgeDetection(image)
    # thres = thres*image
    head,tail = find_edge(thres)
    # print head,tail
    # cv2.imshow("edge",thres)
    tail = tail+5
    if tail>135:
        tail = 135
    image = image[0:35,head:tail]
    image = cv2.resize(image, (int(136), int(36)))
    return image


#打上boundingbox和标签
def drawRectBox(image,rect,addText):
    cv2.rectangle(image, (int(rect[0]), int(rect[1])), (int(rect[0] + rect[2]), int(rect[1] + rect[3])), (0,0, 255), 2, cv2.LINE_AA)
    cv2.rectangle(image, (int(rect[0]-1), int(rect[1])-16), (int(rect[0] + 115), int(rect[1])), (0, 0, 255), -1, cv2.LINE_AA)

    img = Image.fromarray(image)
    draw = ImageDraw.Draw(img)
    #draw.text((int(rect[0]+1), int(rect[1]-16)), addText.decode("utf-8"), (255, 255, 255), font=fontC)
    draw.text((int(rect[0]+1), int(rect[1]-16)), addText, (255, 255, 255), font=fontC)
    imagex = np.array(img)

    return imagex


from . import cache
from . import finemapping_vertical as fv

def RecognizePlateJson(image):
    images = detect.detectPlateRough(image,image.shape[0],top_bottom_padding_rate=0.1)
    jsons = []
    for j,plate in enumerate(images):
        plate,rect,origin_plate =plate
        res, confidence = e2e.recognizeOne(origin_plate)
        print("res",res)

        cv2.imwrite("./"+str(j)+"_rough.jpg",plate)

        # print "车牌类型:",ptype
        # plate = cv2.cvtColor(plate, cv2.COLOR_RGB2GRAY)
        plate  =cv2.resize(plate,(136,int(36*2.5)))
        t1 = time.time()

        ptype = td.SimplePredict(plate)
        if ptype>0 and ptype<4:
            plate = cv2.bitwise_not(plate)
        # demo = verticalEdgeDetection(plate)

        image_rgb = fm.findContoursAndDrawBoundingBox(plate)
        image_rgb = fv.finemappingVertical(image_rgb)
        cache.verticalMappingToFolder(image_rgb)
        # print time.time() - t1,"校正"
        print("e2e:",e2e.recognizeOne(image_rgb)[0])
        image_gray = cv2.cvtColor(image_rgb,cv2.COLOR_BGR2GRAY)

        cv2.imwrite("./"+str(j)+".jpg",image_gray)
        # image_gray = horizontalSegmentation(image_gray)

        t2 = time.time()
        res, confidence = e2e.recognizeOne(image_rgb)
        res_json = {}
        if confidence  > 0.6:
            res_json["Name"] = res
            res_json["Type"] = td.plateType[ptype]
            res_json["Confidence"] = confidence;
            res_json["x"] = int(rect[0])
            res_json["y"] = int(rect[1])
            res_json["w"] = int(rect[2])
            res_json["h"] = int(rect[3])
            jsons.append(res_json)
    print(json.dumps(jsons,ensure_ascii=False,encoding="gb2312"))

    return json.dumps(jsons,ensure_ascii=False,encoding="gb2312")




def SimpleRecognizePlateByE2E(image):
    t0 = time.time()
    images = detect.detectPlateRough(image,image.shape[0],top_bottom_padding_rate=0.1)
    res_set = []
    for j,plate in enumerate(images):
        plate, rect, origin_plate  =plate
        # plate = cv2.cvtColor(plate, cv2.COLOR_RGB2GRAY)
        plate  =cv2.resize(plate,(136,36*2))
        res,confidence = e2e.recognizeOne(origin_plate)
        print("res",res)

        t1 = time.time()
        ptype = td.SimplePredict(plate)
        if ptype>0 and ptype<5:
            # pass
            plate = cv2.bitwise_not(plate)
        image_rgb = fm.findContoursAndDrawBoundingBox(plate)
        image_rgb = fv.finemappingVertical(image_rgb)
        image_rgb = fv.finemappingVertical(image_rgb)
        cache.verticalMappingToFolder(image_rgb)
        cv2.imwrite("./"+str(j)+".jpg",image_rgb)
        res,confidence = e2e.recognizeOne(image_rgb)
        print(res,confidence)
        res_set.append([[],res,confidence])

        if confidence>0.7:
            image = drawRectBox(image, rect, res+" "+str(round(confidence,3)))
    return image,res_set


def SimpleRecognizePlate(image):
    t0 = time.time()
    images = detect.detectPlateRough(image,image.shape[0],top_bottom_padding_rate=0.1)
    res_set = []
    for j,plate in enumerate(images):
        plate, rect, origin_plate  =plate
        # plate = cv2.cvtColor(plate, cv2.COLOR_RGB2GRAY)
        plate  =cv2.resize(plate,(136,36*2))
        t1 = time.time()

        ptype = td.SimplePredict(plate)
        if ptype>0 and ptype<5:
            plate = cv2.bitwise_not(plate)

        image_rgb = fm.findContoursAndDrawBoundingBox(plate)

        image_rgb = fv.finemappingVertical(image_rgb)
        cache.verticalMappingToFolder(image_rgb)
        print("e2e:", e2e.recognizeOne(image_rgb))
        image_gray = cv2.cvtColor(image_rgb,cv2.COLOR_RGB2GRAY)

        # image_gray = horizontalSegmentation(image_gray)
        cv2.imshow("image_gray",image_gray)
        # cv2.waitKey()

        cv2.imwrite("./"+str(j)+".jpg",image_gray)
        # cv2.imshow("image",image_gray)
        # cv2.waitKey(0)
        print("校正",time.time() - t1,"s")
        # cv2.imshow("image,",image_gray)
        # cv2.waitKey(0)
        t2 = time.time()
        val = segmentation.slidingWindowsEval(image_gray)
        # print val
        print("分割和识别",time.time() - t2,"s")
        if len(val)==3:
            blocks, res, confidence = val
            if confidence/7>0.7:
                image =  drawRectBox(image,rect,res)
                res_set.append(res)
                for i,block in enumerate(blocks):

                    block_ = cv2.resize(block,(25,25))
                    block_ = cv2.cvtColor(block_,cv2.COLOR_GRAY2BGR)
                    image[j * 25:(j * 25) + 25, i * 25:(i * 25) + 25] = block_
                    if image[j*25:(j*25)+25,i*25:(i*25)+25].shape == block_.shape:
                        pass


            if confidence>0:
                print("车牌:",res,"置信度:",confidence/7)
            else:
                pass

                # print "不确定的车牌:", res, "置信度:", confidence

    print(time.time() - t0,"s")
    return image,res_set




