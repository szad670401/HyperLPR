import os
import hyperlpr.detect as hyperDetect
import hyperlpr.config as hyperConfig
import cv2
import argparse
import sys

parser = argparse.ArgumentParser()

debugInfo = hyperConfig.configuration["global"]["debug"]
testPath = hyperConfig.configuration["detectTest"]["detectPath"]
outPath = hyperConfig.configuration["detectTest"]["outputPath"]

def drawBoundingBox(originImage,rect):
    cv2.rectangle(originImage, (int(rect[0]), int(rect[1])), (int(rect[0] + rect[2]), int(rect[1] + rect[3])), (0, 0, 255), 2,
                  cv2.LINE_AA)
    return originImage


#detect Plate in image batch
def detectPlateBatchTest(filepath):
    for filename in os.listdir(filepath):
        if filename.endswith(".jpg") or filename.endswith(".png"):
            fileFullPath = os.path.join(filepath,filename)
            image = cv2.imread(fileFullPath)
            image_c = image.copy()
            Plates = hyperDetect.detectPlateRough(image_c, image_c.shape[0], top_bottom_padding_rate=0.1)
            pathName = filename.split('.')[0]
            if debugInfo:
                if len(Plates) != 0:
                    if os.path.exists(os.path.join(outPath,pathName)) == False:
                        os.mkdir(os.path.join(outPath,pathName))
            for i,plate in enumerate(Plates):
                rect = plate[1]
                region = plate[2]
                if debugInfo:
                    cv2.imwrite(os.path.join(outPath,pathName,"region_"+str(i)+"_"+pathName+".png"),region)
                    drawBoundingBox(image_c,rect)
            if debugInfo:
                cv2.imwrite(os.path.join(outPath,pathName,"out_"+pathName+".png"),image_c)


def main(args):
    if args.type == 'batch':
        detectPlateBatchTest(testPath)
    else:
        print "type: "+args.type+" not found!\n"
        print parser.print_help()


def parse_arguments(argv):
    parser.add_argument('--type',type=str,help='detect Plate type{batch},default is batch',default='batch')
    return parser.parse_args(argv)

if __name__ == "__main__":
    main(parse_arguments(sys.argv[1:]))

    #detectPlateBatchTest(testPath)
