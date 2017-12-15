#coding=utf-8
import cv2
import numpy as np


from . import niblack_thresholding as nt

from . import deskew

def fitLine_ransac(pts,zero_add = 0 ):
    if len(pts)>=2:
        [vx, vy, x, y] = cv2.fitLine(pts, cv2.DIST_HUBER, 0, 0.01, 0.01)
        lefty = int((-x * vy / vx) + y)
        righty = int(((136- x) * vy / vx) + y)
        return lefty+30+zero_add,righty+30+zero_add
    return 0,0



#精定位算法
def findContoursAndDrawBoundingBox(image_rgb):


    line_upper  = [];
    line_lower = [];

    line_experiment = []
    grouped_rects = []
    gray_image = cv2.cvtColor(image_rgb,cv2.COLOR_BGR2GRAY)

    # for k in np.linspace(-1.5, -0.2,10):
    for k in np.linspace(-50, 0, 15):

        # thresh_niblack = threshold_niblack(gray_image, window_size=21, k=k)
        # binary_niblack = gray_image > thresh_niblack
        # binary_niblack = binary_niblack.astype(np.uint8) * 255

        binary_niblack = cv2.adaptiveThreshold(gray_image,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,17,k)
        # cv2.imshow("image1",binary_niblack)
        # cv2.waitKey(0)
        imagex, contours, hierarchy = cv2.findContours(binary_niblack.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            bdbox = cv2.boundingRect(contour)
            if (bdbox[3]/float(bdbox[2])>0.7 and bdbox[3]*bdbox[2]>100 and bdbox[3]*bdbox[2]<1200) or (bdbox[3]/float(bdbox[2])>3 and bdbox[3]*bdbox[2]<100):
                # cv2.rectangle(rgb,(bdbox[0],bdbox[1]),(bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]),(255,0,0),1)
                line_upper.append([bdbox[0],bdbox[1]])
                line_lower.append([bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]])

                line_experiment.append([bdbox[0],bdbox[1]])
                line_experiment.append([bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]])
                # grouped_rects.append(bdbox)

    rgb = cv2.copyMakeBorder(image_rgb,30,30,0,0,cv2.BORDER_REPLICATE)
    leftyA, rightyA = fitLine_ransac(np.array(line_lower),3)
    rows,cols = rgb.shape[:2]

    # rgb = cv2.line(rgb, (cols - 1, rightyA), (0, leftyA), (0, 0, 255), 1,cv2.LINE_AA)

    leftyB, rightyB = fitLine_ransac(np.array(line_upper),-3)

    rows,cols = rgb.shape[:2]

    # rgb = cv2.line(rgb, (cols - 1, rightyB), (0, leftyB), (0,255, 0), 1,cv2.LINE_AA)
    pts_map1  = np.float32([[cols - 1, rightyA], [0, leftyA],[cols - 1, rightyB], [0, leftyB]])
    pts_map2 = np.float32([[136,36],[0,36],[136,0],[0,0]])
    mat = cv2.getPerspectiveTransform(pts_map1,pts_map2)
    image = cv2.warpPerspective(rgb,mat,(136,36),flags=cv2.INTER_CUBIC)
    image,M = deskew.fastDeskew(image)

    return image



#多级
def findContoursAndDrawBoundingBox2(image_rgb):


    line_upper  = [];
    line_lower = [];

    line_experiment = []

    grouped_rects = []

    gray_image = cv2.cvtColor(image_rgb,cv2.COLOR_BGR2GRAY)

    for k in np.linspace(-1.6, -0.2,10):
    # for k in np.linspace(-15, 0, 15):
    # #
    #     thresh_niblack = threshold_niblack(gray_image, window_size=21, k=k)
    #     binary_niblack = gray_image > thresh_niblack
    #     binary_niblack = binary_niblack.astype(np.uint8) * 255

        binary_niblack = nt.niBlackThreshold(gray_image,19,k)
        # cv2.imshow("binary_niblack_opencv",binary_niblack_)
        # cv2.imshow("binary_niblack_skimage", binary_niblack)

        # cv2.waitKey(0)
        imagex, contours, hierarchy = cv2.findContours(binary_niblack.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)

        for contour in contours:
            bdbox = cv2.boundingRect(contour)
            if (bdbox[3]/float(bdbox[2])>0.7 and bdbox[3]*bdbox[2]>100 and bdbox[3]*bdbox[2]<1000) or (bdbox[3]/float(bdbox[2])>3 and bdbox[3]*bdbox[2]<100):
                # cv2.rectangle(rgb,(bdbox[0],bdbox[1]),(bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]),(255,0,0),1)
                line_upper.append([bdbox[0],bdbox[1]])
                line_lower.append([bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]])

                line_experiment.append([bdbox[0],bdbox[1]])
                line_experiment.append([bdbox[0]+bdbox[2],bdbox[1]+bdbox[3]])
                # grouped_rects.append(bdbox)

    rgb = cv2.copyMakeBorder(image_rgb,30,30,0,0,cv2.BORDER_REPLICATE)
    leftyA, rightyA = fitLine_ransac(np.array(line_lower),2)
    rows,cols = rgb.shape[:2]

    # rgb = cv2.line(rgb, (cols - 1, rightyA), (0, leftyA), (0, 0, 255), 1,cv2.LINE_AA)

    leftyB, rightyB = fitLine_ransac(np.array(line_upper),-4)

    rows,cols = rgb.shape[:2]

    # rgb = cv2.line(rgb, (cols - 1, rightyB), (0, leftyB), (0,255, 0), 1,cv2.LINE_AA)
    pts_map1  = np.float32([[cols - 1, rightyA], [0, leftyA],[cols - 1, rightyB], [0, leftyB]])
    pts_map2 = np.float32([[136,36],[0,36],[136,0],[0,0]])
    mat = cv2.getPerspectiveTransform(pts_map1,pts_map2)
    image = cv2.warpPerspective(rgb,mat,(136,36),flags=cv2.INTER_CUBIC)
    image,M= deskew.fastDeskew(image)


    return image
