#coding=utf-8
from keras.models import Sequential
from keras.layers import Dense, Dropout, Activation, Flatten
from keras.layers import Conv2D,MaxPool2D
from keras.optimizers import SGD
from keras import backend as K

K.image_data_format()


import cv2
import numpy as np



index = {u"京": 0, u"沪": 1, u"津": 2, u"渝": 3, u"冀": 4, u"晋": 5, u"蒙": 6, u"辽": 7, u"吉": 8, u"黑": 9, u"苏": 10, u"浙": 11, u"皖": 12,
         u"闽": 13, u"赣": 14, u"鲁": 15, u"豫": 16, u"鄂": 17, u"湘": 18, u"粤": 19, u"桂": 20, u"琼": 21, u"川": 22, u"贵": 23, u"云": 24,
         u"藏": 25, u"陕": 26, u"甘": 27, u"青": 28, u"宁": 29, u"新": 30, u"0": 31, u"1": 32, u"2": 33, u"3": 34, u"4": 35, u"5": 36,
         u"6": 37, u"7": 38, u"8": 39, u"9": 40, u"A": 41, u"B": 42, u"C": 43, u"D": 44, u"E": 45, u"F": 46, u"G": 47, u"H": 48,
         u"J": 49, u"K": 50, u"L": 51, u"M": 52, u"N": 53, u"P": 54, u"Q": 55, u"R": 56, u"S": 57, u"T": 58, u"U": 59, u"V": 60,
         u"W": 61, u"X": 62, u"Y": 63, u"Z": 64,u"港":65,u"学":66 ,u"O":67 ,u"使":68,u"警":69,u"澳":70,u"挂":71};

chars = ["京", "沪", "津", "渝", "冀", "晋", "蒙", "辽", "吉", "黑", "苏", "浙", "皖", "闽", "赣", "鲁", "豫", "鄂", "湘", "粤", "桂",
             "琼", "川", "贵", "云", "藏", "陕", "甘", "青", "宁", "新", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A",
             "B", "C", "D", "E", "F", "G", "H", "J", "K", "L", "M", "N", "P",
         "Q", "R", "S", "T", "U", "V", "W", "X",
             "Y", "Z","港","学","O","使","警","澳","挂" ];



def Getmodel_tensorflow(nb_classes):
    # nb_classes = len(charset)

    img_rows, img_cols = 23, 23
    # number of convolutional filters to use
    nb_filters = 32
    # size of pooling area for max pooling
    nb_pool = 2
    # convolution kernel size
    nb_conv = 3

    # x = np.load('x.npy')
    
    # y = np_utils.to_categorical(range(3062)*45*5*2, nb_classes)
    # weight = ((type_class - np.arange(type_class)) / type_class + 1) ** 3
    # weight = dict(zip(range(3063), weight / weight.mean()))  # 调整权重，高频字优先

    model = Sequential()
    model.add(Conv2D(32, (5, 5),input_shape=(img_rows, img_cols,1)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Dropout(0.25))
    model.add(Conv2D(32, (3, 3)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Dropout(0.25))
    model.add(Conv2D(512, (3, 3)))
    # model.add(Activation('relu'))
    # model.add(MaxPooling2D(pool_size=(nb_pool, nb_pool)))
    # model.add(Dropout(0.25))
    model.add(Flatten())
    model.add(Dense(512))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(nb_classes))
    model.add(Activation('softmax'))
    model.compile(loss='categorical_crossentropy',
                  optimizer='adam',
                  metrics=['accuracy'])
    return model




def Getmodel_ch(nb_classes):
    # nb_classes = len(charset)

    img_rows, img_cols = 23, 23
    # number of convolutional filters to use
    nb_filters = 32
    # size of pooling area for max pooling
    nb_pool = 2
    # convolution kernel size
    nb_conv = 3

    # x = np.load('x.npy')
    # y = np_utils.to_categorical(range(3062)*45*5*2, nb_classes)
    # weight = ((type_class - np.arange(type_class)) / type_class + 1) ** 3
    # weight = dict(zip(range(3063), weight / weight.mean()))  # 调整权重，高频字优先

    model = Sequential()
    model.add(Conv2D(32, (5, 5),input_shape=(img_rows, img_cols,1)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Dropout(0.25))
    model.add(Conv2D(32, (3, 3)))
    model.add(Activation('relu'))
    model.add(MaxPool2D(pool_size=(nb_pool, nb_pool)))
    model.add(Dropout(0.25))
    model.add(Conv2D(512, (3, 3)))
    # model.add(Activation('relu'))
    # model.add(MaxPooling2D(pool_size=(nb_pool, nb_pool)))
    # model.add(Dropout(0.25))
    model.add(Flatten())
    model.add(Dense(756))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(nb_classes))
    model.add(Activation('softmax'))
    model.compile(loss='categorical_crossentropy',
                  optimizer='adam',
                  metrics=['accuracy'])
    return model



model  = Getmodel_tensorflow(65)
#构建网络

model_ch = Getmodel_ch(31)

model_ch.load_weights("./model/char_chi_sim.h5")
# model_ch.save_weights("./model/char_chi_sim.h5")
model.load_weights("./model/char_rec.h5")
# model.save("./model/char_rec.h5")


def SimplePredict(image,pos):
    image = cv2.resize(image, (23, 23))
    image = cv2.equalizeHist(image)
    image = image.astype(np.float) / 255
    image -= image.mean()
    image = np.expand_dims(image, 3)
    if pos!=0:
        res = np.array(model.predict(np.array([image]))[0])
    else:
        res = np.array(model_ch.predict(np.array([image]))[0])

    zero_add = 0 ;

    if pos==0:
        res = res[:31]
    elif pos==1:
        res = res[31+10:65]
        zero_add = 31+10
    else:
        res = res[31:]
        zero_add = 31

    max_id = res.argmax()


    return res.max(),chars[max_id+zero_add],max_id+zero_add

