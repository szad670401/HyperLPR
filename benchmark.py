#coding=utf-8
import os
import numpy as np
import cv2
import json
from hyperlpr import  pipline as  pp
import sys
from Levenshtein import StringMatcher as sm

reload(sys)
sys.setdefaultencoding("utf-8")
parent= "you folder"

def comparestring(a,b):
    g = 0
    if len(a) == len(b):
        for x,y in zip(a,b):
            if x!=y:
                g+=1
    return g
count = 0 ;
count_p = 0
count_d = 0
count_lev = 0
count_undetected = 0
roi = [470,400,650,580]
for filename in os.listdir(parent):

    path = os.path.join(parent,filename)
    print path

    if path.endswith(".jpg") or path.endswith(".png"):
        ics,name = os.path.split(path)


        name,ext = name.split(".")

        image =  cv2.imread(path)
        image = image[roi[1]:roi[1]+roi[3],roi[0]:roi[0]+roi[2]]
        # cv2.imshow("test",image)
        # cv2.waitKey(0)
        info,dataset = pp.SimpleRecognizePlate(image)
        ext = ext.strip()
        name = name.strip()

        if len(dataset)==0:
            count_undetected +=1

            # cv2.imwrite("./cache/bad2/" + name + ".png", image)

        for one in dataset:

            # p = sm.StringMatcher(seq1=one.encode("utf-8"),seq2=name.encode("utf-8"))
            A = one.decode("utf-8")
            B = name.decode("utf-8")
            print one.decode("utf-8"),"<->",name.decode("utf-8"),"编辑距离:",comparestring(A,B)
            if  comparestring(A,B)<3:
                count_lev+=1
            else:
                cv2.imwrite("./cache/bad2/"+B+"->"+A+".png",image)

            if one.decode("utf-8") == name.decode("utf-8"):
                count_p+=1
                break
            else:
                print "error",one.decode("utf-8"), name.decode("utf-8")
                count_d+=1
                # cv2.imshow("image",image)
                # cv2.waitKey(0)
                # break
        count+=1
        print count_p / float(count),"编辑距离[1]:",count_lev/float(count),u"识出",count_p,u"总数",count,u"未识出",count_d,u"未检测出",count_undetected
        if count_p+count_d+count_undetected!=count:
            print dataset,len(dataset)
            # exit(0)
