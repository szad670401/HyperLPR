from hyperlpr import *
import cv2
image = cv2.imread("test.png")
print(HyperLPR_plate_recognition(image,16,charSelectionDeskew=False))
