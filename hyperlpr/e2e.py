#coding=utf-8
from keras import backend as K
from keras.models import load_model
from keras.layers import *
import numpy as np
import random
import string

import cv2
import e2emodel as model
chars = [u"京", u"沪", u"津", u"渝", u"冀", u"晋", u"蒙", u"辽", u"吉", u"黑", u"苏", u"浙", u"皖", u"闽", u"赣", u"鲁", u"豫", u"鄂", u"湘", u"粤", u"桂",
             u"琼", u"川", u"贵", u"云", u"藏", u"陕", u"甘", u"青", u"宁", u"新", u"0", u"1", u"2", u"3", u"4", u"5", u"6", u"7", u"8", u"9", u"A",
             u"B", u"C", u"D", u"E", u"F", u"G", u"H", u"J", u"K", u"L", u"M", u"N", u"P", u"Q", u"R", u"S", u"T", u"U", u"V", u"W", u"X",
             u"Y", u"Z",u"港",u"学",u"使",u"警",u"澳",u"挂",u"军",u"北",u"南",u"广",u"沈",u"兰",u"成",u"济",u"海",u"民",u"航",u"空"
             ];
pred_model = model.construct_model("./model/ocr_plate_all_w_rnn_2.h5",)
import time



def fastdecode(y_pred):
    results = ""
    confidence = 0.0
    table_pred = y_pred.reshape(-1, len(chars)+1)

    res = table_pred.argmax(axis=1)

    for i,one in enumerate(res):
        if one<len(chars) and (i==0 or (one!=res[i-1])):
            results+= chars[one]
            confidence+=table_pred[i][one]
    confidence/= len(results)
    return results,confidence

def recognizeOne(src):
    # x_tempx= cv2.imread(src)
    x_tempx = src
    # x_tempx = cv2.bitwise_not(x_tempx)
    x_temp = cv2.resize(x_tempx,( 160,40))
    x_temp = x_temp.transpose(1, 0, 2)
    t0 = time.time()
    y_pred = pred_model.predict(np.array([x_temp]))
    y_pred = y_pred[:,2:,:]
    # plt.imshow(y_pred.reshape(16,66))
    # plt.show()

    #
    # cv2.imshow("x_temp",x_tempx)
    # cv2.waitKey(0)
    return fastdecode(y_pred)
#
#
# import os
#
# path = "/Users/yujinke/PycharmProjects/HyperLPR_Python_web/cache/finemapping"
# for filename in os.listdir(path):
#     if filename.endswith(".png") or filename.endswith(".jpg") or filename.endswith(".bmp"):
#         x = os.path.join(path,filename)
#         recognizeOne(x)
#         # print time.time() - t0
#
#         # cv2.imshow("x",x)
#         # cv2.waitKey()
