#coding=utf-8
import cv2
import numpy as np

# from matplotlib import pyplot as plt
import scipy.ndimage.filters as f
import scipy

import time
import scipy.signal as l





from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Conv2D, MaxPool2D
from keras.optimizers import SGD
from keras import backend as K

K.image_data_format()


def Getmodel_tensorflow(nb_classes):
    # nb_classes = len(charset)
    img_rows, img_cols = 23, 23
    # number of convolutional filters to use
    nb_filters = 16
    # size of pooling area for max pooling
    nb_pool = 2
    # convolution kernel size
    nb_conv = 3
    # x = np.load('x.npy')
    # y = np_utils.to_categorical(range(3062)*45*5*2, nb_classes)
    # weight = ((type_class - np.arange(type_class)) / type_class + 1) ** 3
    # weight = dict(zip(range(3063), weight / weight.mean()))  # 调整权重，高频字优先

    model = Sequential()
    model.add(Conv2D(nb_filters, (nb_conv, nb_conv),input_shape=(img_rows, img_cols,1)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Conv2D(nb_filters, (nb_conv, nb_conv)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Flatten())
    model.add(Dense(256))
    model.add(Dropout(0.5))

    model.add(Activation('relu'))
    model.add(Dense(nb_classes))
    model.add(Activation('softmax'))
    model.compile(loss='categorical_crossentropy',
                  optimizer='sgd',
                  metrics=['accuracy'])
    return model



def Getmodel_tensorflow_light(nb_classes):
    # nb_classes = len(charset)
    img_rows, img_cols = 23, 23
    # number of convolutional filters to use
    nb_filters = 8
    # size of pooling area for max pooling
    nb_pool = 2
    # convolution kernel size
    nb_conv = 3
    # x = np.load('x.npy')
    # y = np_utils.to_categorical(range(3062)*45*5*2, nb_classes)
    # weight = ((type_class - np.arange(type_class)) / type_class + 1) ** 3
    # weight = dict(zip(range(3063), weight / weight.mean()))  # 调整权重，高频字优先

    model = Sequential()
    model.add(Conv2D(nb_filters, (nb_conv, nb_conv),input_shape=(img_rows, img_cols, 1)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Conv2D(nb_filters, (nb_conv * 2, nb_conv * 2)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Flatten())
    model.add(Dense(32))
    # model.add(Dropout(0.25))

    model.add(Activation('relu'))
    model.add(Dense(nb_classes))
    model.add(Activation('softmax'))
    model.compile(loss='categorical_crossentropy',
                  optimizer='adam',
                  metrics=['accuracy'])
    return model




model  = Getmodel_tensorflow_light(3)
model2  = Getmodel_tensorflow(3)

import os
model.load_weights("./model/char_judgement1.h5")
# model.save("./model/char_judgement1.h5")
model2.load_weights("./model/char_judgement.h5")
# model2.save("./model/char_judgement.h5")


model = model2
def get_median(data):
   data = sorted(data)
   size = len(data)
   # print size

   if size % 2 == 0: # 判断列表长度为偶数
    median = (data[size//2]+data[size//2-1])//2
    data[0] = median
   if size % 2 == 1: # 判断列表长度为奇数
    median = data[(size-1)//2]
    data[0] = median
   return data[0]
import time

def searchOptimalCuttingPoint(rgb,res_map,start,width_boundingbox,interval_range):
    t0  = time.time()
    #
    # for x in xrange(10):
    #     res_map = np.vstack((res_map,res_map[-1]))
    length = res_map.shape[0]
    refine_s = -2;

    if width_boundingbox>20:
        refine_s = -9
    score_list = []
    interval_big = int(width_boundingbox * 0.3)  #
    p = 0
    for zero_add in range(start,start+50,3):
        # for interval_small in xrange(-0,width_boundingbox/2):
        for i in range(-8,int(width_boundingbox/1)-8):
            for refine in range(refine_s, int(width_boundingbox/2+3)):
                p1 = zero_add# this point is province
                p2 = p1 + width_boundingbox +refine #
                p3 = p2 + width_boundingbox + interval_big+i+1
                p4 = p3 + width_boundingbox +refine
                p5 = p4 + width_boundingbox +refine
                p6 = p5 + width_boundingbox +refine
                p7 = p6 + width_boundingbox +refine
                if p7>=length:
                    continue
                score = res_map[p1][2]*3 -(res_map[p3][1]+res_map[p4][1]+res_map[p5][1]+res_map[p6][1]+res_map[p7][1])+7
                # print score
                score_list.append([score,[p1,p2,p3,p4,p5,p6,p7]])
                p+=1
    print(p)

    score_list = sorted(score_list , key=lambda x:x[0])
    # for one in score_list[-1][1]:
    #     cv2.line(debug,(one,0),(one,36),(255,0,0),1)
    # #
    # cv2.imshow("one",debug)
    # cv2.waitKey(0)
    #
    print("寻找最佳点",time.time()-t0)
    return score_list[-1]


import sys

sys.path.append('../')
from . import recognizer as cRP
from . import niblack_thresholding as nt

def refineCrop(sections,width=16):
    new_sections = []
    for section in sections:
        # cv2.imshow("section¡",section)

        # cv2.blur(section,(3,3),3)

        sec_center = np.array([section.shape[1]/2,section.shape[0]/2])
        binary_niblack = nt.niBlackThreshold(section,17,-0.255)
        imagex, contours, hierarchy  = cv2.findContours(binary_niblack,cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        boxs = []
        for contour in contours:
            x,y,w,h = cv2.boundingRect(contour)

            ratio = w/float(h)
            if ratio<1 and h>36*0.4 and y<16\
                    :
                box = [x,y,w,h]

                boxs.append([box,np.array([x+w/2,y+h/2])])
                # cv2.rectangle(section,(x,y),(x+w,y+h),255,1)




        # print boxs

        dis_ = np.array([ ((one[1]-sec_center)**2).sum() for one in boxs])
        if len(dis_)==0:
            kernal = [0, 0, section.shape[1], section.shape[0]]
        else:
            kernal = boxs[dis_.argmin()][0]

        center_c  = (kernal[0]+kernal[2]/2,kernal[1]+kernal[3]/2)
        w_2 = int(width/2)
        h_2 = kernal[3]/2

        if center_c[0] - w_2< 0:
            w_2 = center_c[0]
        new_box = [center_c[0] - w_2,kernal[1],width,kernal[3]]
        # print new_box[2]/float(new_box[3])
        if new_box[2]/float(new_box[3])>0.5:
            # print "异常"
            h = int((new_box[2]/0.35 )/2)
            if h>35:
                h = 35
            new_box[1] = center_c[1]- h
            if new_box[1]<0:
                new_box[1] = 1
            new_box[3] = h*2
        
        section  = section[int(new_box[1]):int(new_box[1]+new_box[3]), int(new_box[0]):int(new_box[0]+new_box[2])]
        # cv2.imshow("section",section)
        # cv2.waitKey(0)
        new_sections.append(section)
        # print new_box
    return new_sections


def slidingWindowsEval(image):
    windows_size = 16;
    stride = 1
    height= image.shape[0]
    t0 = time.time()
    data_sets = []

    for i in range(0,image.shape[1]-windows_size+1,stride):
        data = image[0:height,i:i+windows_size]
        data = cv2.resize(data,(23,23))
        # cv2.imshow("image",data)
        data = cv2.equalizeHist(data)
        data = data.astype(np.float)/255
        data=  np.expand_dims(data,3)
        data_sets.append(data)

    res = model2.predict(np.array(data_sets))
    print("分割",time.time() - t0)

    pin = res
    p = 1 -  (res.T)[1]
    p = f.gaussian_filter1d(np.array(p,dtype=np.float),3)
    lmin = l.argrelmax(np.array(p),order = 3)[0]
    interval = []
    for i in range(len(lmin)-1):
        interval.append(lmin[i+1]-lmin[i])

    if(len(interval)>3):
        mid  = get_median(interval)
    else:
        return []
    pin = np.array(pin)
    res =  searchOptimalCuttingPoint(image,pin,0,mid,3)

    cutting_pts = res[1]
    last =  cutting_pts[-1] + mid
    if last < image.shape[1]:
        cutting_pts.append(last)
    else:
        cutting_pts.append(image.shape[1]-1)
    name = ""
    confidence =0.00
    seg_block = []
    for x in range(1,len(cutting_pts)):
        if x != len(cutting_pts)-1 and x!=1:
            section = image[0:36,cutting_pts[x-1]-2:cutting_pts[x]+2]
        elif  x==1:
            c_head = cutting_pts[x - 1]- 2
            if c_head<0:
                c_head=0
            c_tail = cutting_pts[x] + 2
            section = image[0:36, c_head:c_tail]
        elif x==len(cutting_pts)-1:
            end = cutting_pts[x]
            diff = image.shape[1]-end
            c_head = cutting_pts[x - 1]
            c_tail = cutting_pts[x]
            if diff<7 :
                section = image[0:36, c_head-5:c_tail+5]
            else:
                diff-=1
                section = image[0:36, c_head - diff:c_tail + diff]
        elif  x==2:
            section = image[0:36, cutting_pts[x - 1] - 3:cutting_pts[x-1]+ mid]
        else:
            section = image[0:36,cutting_pts[x-1]:cutting_pts[x]]
        seg_block.append(section)
    refined = refineCrop(seg_block,mid-1)

    t0 = time.time()
    for i,one in enumerate(refined):
        res_pre = cRP.SimplePredict(one, i )
        # cv2.imshow(str(i),one)
        # cv2.waitKey(0)
        confidence+=res_pre[0]
        name+= res_pre[1]
    print("字符识别",time.time() - t0)

    return refined,name,confidence
