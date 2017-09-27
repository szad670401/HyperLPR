#coding=utf-8
from keras.layers import Conv2D, Input,MaxPool2D, Reshape,Activation,Flatten, Dense
from keras.models import Model, Sequential
from keras.layers.advanced_activations import PReLU
from keras.optimizers import adam
import numpy as np

import cv2

def getModel():
    input = Input(shape=[12, 50, 3])  # change this shape to [None,None,3] to enable arbitraty shape input
    x = Conv2D(10, (3, 3), strides=1, padding='valid', name='conv1')(input)
    x = PReLU(shared_axes=[1, 2], name='prelu1')(x)
    x = MaxPool2D(pool_size=2)(x)
    x = Conv2D(16, (3, 3), strides=1, padding='valid', name='conv2')(x)
    x = PReLU(shared_axes=[1, 2], name='prelu2')(x)
    x = Conv2D(32, (3, 3), strides=1, padding='valid', name='conv3')(x)
    x = PReLU(shared_axes=[1, 2], name='prelu3')(x)
    x = Flatten()(x)
    output = Dense(2)(x)
    output = PReLU(name='prelu4')(output)
    model = Model([input], [output])
    return model

model = getModel()
# model.load_weights("./model/model12.h5")


def finemappingVertical(image):
    resized = cv2.resize(image,(50,12))
    resized = resized.astype(np.float)/255
    res= model.predict(np.array([resized]))[0]
    res  =res*image.shape[1]
    res = res.astype(np.int)
    image = image[0:35,res[0]+4:res[1]]
    image = cv2.resize(image, (int(136), int(36)))
    return image