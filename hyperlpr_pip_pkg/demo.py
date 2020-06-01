import cv2
from hyperlpr import *

image = cv2.imread("./test_images/test.jpg")
print(HyperLPR_plate_recognition(image, 16, charSelectionDeskew=False, DB=False))
