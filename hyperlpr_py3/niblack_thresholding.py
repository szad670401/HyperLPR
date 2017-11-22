import cv2
import numpy as np



def niBlackThreshold(  src,  blockSize,  k,  binarizationMethod= 0 ):
    mean = cv2.boxFilter(src,cv2.CV_32F,(blockSize, blockSize),borderType=cv2.BORDER_REPLICATE)
    sqmean = cv2.sqrBoxFilter(src, cv2.CV_32F, (blockSize, blockSize), borderType = cv2.BORDER_REPLICATE)
    variance = sqmean - (mean*mean)
    stddev  = np.sqrt(variance)
    thresh = mean + stddev * float(-k)
    thresh = thresh.astype(src.dtype)
    k = (src>thresh)*255
    k = k.astype(np.uint8)
    return k


# cv2.imshow()