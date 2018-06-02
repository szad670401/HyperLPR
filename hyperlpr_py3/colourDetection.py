# -- coding: UTF-8
import cv2
import matplotlib.pyplot as plt
from sklearn.cluster import KMeans
import os

boundaries = [
    ([100,80,0],[240,220,110]), # yellow
    ([0,40,50],[110,180,250]), # blue
    ([0,60,0],[60,160,70]), # green
]
color_attr = ["黄牌","蓝牌",'绿牌','白牌','黑牌']

threhold_green = 13
threhold_blue = 13
threhold_yellow1 = 50
threhold_yellow2 = 70

# plt.figure()
# plt.axis("off")
# plt.imshow(image)
# plt.show()

import numpy as np
def centroid_histogram(clt):
    numLabels = np.arange(0, len(np.unique(clt.labels_)) + 1)
    (hist, _) = np.histogram(clt.labels_, bins=numLabels)

    # normalize the histogram, such that it sums to one
    hist = hist.astype("float")
    hist /= hist.sum()

    # return the histogram
    return hist


def plot_colors(hist, centroids):
    bar = np.zeros((50, 300, 3), dtype="uint8")
    startX = 0

    for (percent, color) in zip(hist, centroids):

        endX = startX + (percent * 300)
        cv2.rectangle(bar, (int(startX), 0), (int(endX), 50),
                      color.astype("uint8").tolist(), -1)
        startX = endX

    # return the bar chart
    return bar

def search_boundaries(color):
    for i,color_bound in enumerate(boundaries):
        if np.all(color >= color_bound[0]) and np.all(color <= color_bound[1]):
            return i
    return -1

def judge_color(color):
    r = color[0]
    g = color[1]
    b = color[2]
    if g - r >= threhold_green and g - b >= threhold_green:
        return 2
    if b - r >= threhold_blue and b - g >= threhold_blue:
        return 1
    if r- b > threhold_yellow2 and g - b > threhold_yellow2:
        return 0
    if r > 200 and b > 200 and g > 200:
        return 3
    if r < 50 and b < 50 and g < 50:
        return 4
    return -1

def judge_plate_color(img):
    image = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    image = image.reshape((image.shape[0] * image.shape[1], 3))
    clt = KMeans(n_clusters=2)
    clt.fit(image)

    hist = centroid_histogram(clt)
    index = np.argmax(hist)
    #print clt.cluster_centers_[index]
    #color_index = search_boundaries(clt.cluster_centers_[index])
    color_index = judge_color(clt.cluster_centers_[index])
    if color_index == -1:
        if index == 0:
            secound_index = 1
        else:
            secound_index = 0
        color_index = judge_color(clt.cluster_centers_[secound_index])

    if color_index == -1:
        print(clt.cluster_centers_)
        bar = plot_colors(hist, clt.cluster_centers_)
        # show our color bart
        plt.figure()
        plt.axis("off")
        plt.imshow(bar)
        plt.show()

    if color_index != -1:
        return color_attr[color_index],clt.cluster_centers_[index]
    else:
        return None,clt.cluster_centers_[index]