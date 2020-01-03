from hyperlpr import *
import cv2
image = cv2.imread("./test_images/test_db2.jpg")
print(HyperLPR_plate_recognition(image,16,charSelectionDeskew=False,DB=True))
