#coding=utf-8
import cv2
import numpy as np
import os
from .table_chs import chars




class LPR():
    def __init__(self,folder):
        """
        Init the recognition instance.

        :param model_detection:    opencv cascade model which detecting license plate.
        :param model_finemapping:    finemapping model which deskew the license plate
        :param model_rec:   CNN based sequence recognition model trained with CTC loss.
        """

        charLocPath= os.path.join(folder,"cascade/char/char_single.xml")
        detectorPath = os.path.join(folder,"cascade/detector/detector_ch.xml")
        detectorPathDB = os.path.join(folder,"cascade/detector/cascade_double.xml")
        modelRecognitionPath = [os.path.join(folder,"dnn/SegmenationFree-Inception.prototxt"),os.path.join(folder,"dnn/SegmenationFree-Inception.caffemodel")]
        modelFineMappingPath= [os.path.join(folder,"dnn/HorizonalFinemapping.prototxt"),os.path.join(folder,"dnn/HorizonalFinemapping.caffemodel")]
        mini_ssd_path= [os.path.join(folder,"dnn/mininet_ssd_v1.prototxt"),os.path.join(folder,"dnn/mininet_ssd_v1.caffemodel")]
        refine_net_path = [os.path.join(folder,"dnn/refinenet.prototxt"),os.path.join(folder,"dnn/refinenet.caffemodel")]

        self.detector = cv2.CascadeClassifier(detectorPath)
        self.detectorDB = cv2.CascadeClassifier(detectorPathDB)
        self.charLoc = cv2.CascadeClassifier(charLocPath)
        self.modelRecognition = cv2.dnn.readNetFromCaffe(*modelRecognitionPath)
        self.ssd_detection = cv2.dnn.readNetFromCaffe(*mini_ssd_path)
        self.refine_net  =  cv2.dnn.readNetFromCaffe(*refine_net_path)

    def detect_ssd(self,im):
        """
        Detect the approximate location of plate via single shot detector based on modified mobilenet.
        :param im:  input image (BGR) .
        :return:  [[cropped,x1,y2,x2,y2] ,... ]
        """
        _im = im.copy()
        pixel_means = [0.406, 0.456, 0.485]
        pixel_stds = [0.225, 0.224, 0.229]
        pixel_scale = 255.0
        rows, cols, c = im.shape
        im_tensor = np.zeros((1, 3, im.shape[0], im.shape[1]))
        im = im.astype(np.float32)
        for i in range(3):
            im_tensor[0, i, :, :] = (im[:, :, 2 - i] / pixel_scale - pixel_means[2 - i]) / pixel_stds[2 - i]
        self.ssd_detection.setInput(im_tensor)
        print(im_tensor.shape)
        cropped_images = []
        cvOut = self.ssd_detection.forward()
        for detection in cvOut[0, 0, :, :]:
            score = float(detection[2])
            if score > 0.5:
                x1 = int(detection[3] * cols)
                y1 = int(detection[4] * rows)
                x2 = int(detection[5] * cols)
                y2 = int(detection[6] * rows)
                x1 = max(x1,0)
                y1 = max(y1,0)
                x2 = min(x2,im.shape[1]-1)
                y2 = min(y2,im.shape[0]-1)
                cropped = _im[y1:y2,x1:x2]
                cropped_images.append([cropped ,[x1,y1,x2,y2]])
        return cropped_images

    def detect_traditional(self,image_gray,resize_h = 720,en_scale =1.1,minSize = 30,DB=True):
        """
        Detect the approximate location of plate via opencv build-in cascade detection.
        :param image_gray:  input single channel image (gray) .
        :param resize_h:  adjust input image size to a fixed size.
        :param en_scale:  the ratio of image between every scale of images in cascade detection.
        :param minSize:  minSize of plate increase this parameter can increase the speed of detection.
        :return:   the results.
        """
        if DB:
            watches = self.detectorDB.detectMultiScale(image_gray, en_scale, 3, minSize=(minSize*4, minSize))
        else:
            watches = self.detector.detectMultiScale(image_gray, en_scale, 3, minSize=(minSize*4, minSize))
        cropped_images = []
        for (x, y, w, h) in watches:
            x -= w * 0.14
            w += w * 0.28
            y -= h * 0.15
            h += h * 0.35
            x1 = int(x)
            y1 = int(y)
            x2 = int(x+w)
            y2 = int(y+h)
            x1 = max(x1,0)
            y1 = max(y1,0)
            x2 = min(x2,image_gray.shape[1]-1)
            y2 = min(y2,image_gray.shape[0]-1)
            cropped = image_gray[y1:y2,x1:x2]
            cropped_images.append([cropped ,[x1,y1,x2,y2]])
        return cropped_images


    def loose_crop(self,image, box, aspect_ratio, padding_ratio=1.7):
        """
        Crop the image with an extend rectangle.
        :param image:  input image (BGR).
        :param box:  origin bounding box.
        :param aspect_ratio:  the aspect ratio that need to keep.
        :param padding_ratio:  padding ratio of origin rectangle.
        :return:   the cropped image
        """
        x1, y1, x2, y2 = box
        cx, cy = ((x2 + x1) // 2, (y2 + y1) // 2)
        if (x2 - x1) / (y2 - y1) > aspect_ratio:
            _w = int((x2 - x1) * padding_ratio)
            _h = int(_w / aspect_ratio)
        else:
            _h = int((y2 - y1) * padding_ratio)
            _w = int(_h * aspect_ratio)
        x1, y1, x2, y2 = cx - _w // 2, cy - _h // 2, cx + _w // 2, cy + _h // 2
        x1 = int(max(x1, 0))
        y1 = int(max(y1, 0))
        cropped = image[y1:y2, x1:x2]
        return cropped


    def decode_ctc(self,y_pred):
        """
        Decode  the results from the last layer of recognition model.
        :param y_pred:  the feature map output last feature map.
        :return: decode results.
        """
        results = ""
        confidence = 0.0
        y_pred = y_pred.T
        table_pred = y_pred
        res = table_pred.argmax(axis=1)
        for i,one in enumerate(res):
            if one<len(chars) and (i==0 or (one!=res[i-1])):
                results+= chars[one]
                confidence+=table_pred[i][one]
        confidence/= len(results)
        return results,confidence

    def fit_ransac(self,pts, zero_add=0):
        """
        fit a line and use RANSAC algorithm to reject outlier.
        :param pts: input pts
        :return: The line border around the image on the location of the point
        """
        if len(pts) >= 2:
            [vx, vy, x, y] = cv2.fitLine(pts, cv2.DIST_HUBER, 0, 0.01, 0.01)
            lefty = int((-x * vy / vx) + y)
            righty = int(((136 - x) * vy / vx) + y)
            return lefty + 30 + zero_add, righty + 30 + zero_add
        return 0, 0

    def fine_mapping(self,image_rgb):
        """
        fit plate upper and lower with multi-threshold method to segment single character.
        :param image_rgb:
        :return: fined image.
        """

        line_upper = [];
        line_lower = [];
        line_experiment = []
        if image_rgb.ndim == 3:
            gray_image = cv2.cvtColor(image_rgb, cv2.COLOR_BGR2GRAY)
        else:
            gray_image = image_rgb

        for k in np.linspace(-50, 0, 16):
            binary_niblack = cv2.adaptiveThreshold(gray_image, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 17,k)
            if cv2.__version__[0] == "4":
                contours, hierarchy = cv2.findContours(binary_niblack.copy(), cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
            else:
                imagex, contours, hierarchy = cv2.findContours(binary_niblack.copy(), cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_SIMPLE)
            for contour in contours:
                bdbox = cv2.boundingRect(contour)
                if ((bdbox[3] / float(bdbox[2]) > 0.7 and bdbox[3] * bdbox[2] > 100 and bdbox[3] * bdbox[2] < 1200) or (
                        bdbox[3] / float(bdbox[2]) > 3 and bdbox[3] * bdbox[2] < 100)):
                    line_upper.append([bdbox[0], bdbox[1]])
                    line_lower.append([bdbox[0] + bdbox[2], bdbox[1] + bdbox[3]])
                    line_experiment.append([bdbox[0], bdbox[1]])
                    line_experiment.append([bdbox[0] + bdbox[2], bdbox[1] + bdbox[3]])
        rgb = cv2.copyMakeBorder(image_rgb, 30, 30, 0, 0, cv2.BORDER_REPLICATE)
        leftyA, rightyA = self.fit_ransac(np.array(line_lower), 3)
        leftyB, rightyB = self.fit_ransac(np.array(line_upper), -3)
        rows, cols = rgb.shape[:2]
        pts_map1 = np.float32([[cols - 1, rightyA], [0, leftyA], [cols - 1, rightyB], [0, leftyB]])
        pts_map2 = np.float32([[136, 36], [0, 36], [136, 0], [0, 0]])
        mat = cv2.getPerspectiveTransform(pts_map1, pts_map2)
        image = cv2.warpPerspective(rgb, mat, (136, 36), flags=cv2.INTER_CUBIC)
        return image


    def fine_mapping_by_selecting(self,image_rgb,line_upper,line_lower ):
        """
        fit plate upper and lower with detecting character boundingbox
        :param image_rgb: input image
        :param line_upper: padding of upper
        :param line_lower: padding of lower
        :return: fined image.
        """


        rgb = cv2.copyMakeBorder(image_rgb, 30, 30, 0, 0, cv2.BORDER_REPLICATE)
        leftyA, rightyA = self.fit_ransac(np.array(line_lower), 3)
        leftyB, rightyB = self.fit_ransac(np.array(line_upper), -3)
        rows, cols = rgb.shape[:2]
        pts_map1 = np.float32([[cols - 1, rightyA], [0, leftyA], [cols - 1, rightyB], [0, leftyB]])
        pts_map2 = np.float32([[136, 36], [0, 36], [136, 0], [0, 0]])
        mat = cv2.getPerspectiveTransform(pts_map1, pts_map2)
        image = cv2.warpPerspective(rgb, mat, (136, 36), flags=cv2.INTER_CUBIC)
        return image

    def to_refine(self,image, pts, scale=3.0):
        """
        refine the image by input points.
        :param image_rgb: input image
        :param pts: points
        """
        x1, y1, x2, y2, x3, y3, x4, y4 = pts.ravel()
        cx, cy = int(128 // 2), int(48 // 2)
        cw = 64
        ch = 24
        tx1 = cx - cw // 2
        ty1 = cy - ch // 2
        tx2 = cx + cw // 2
        ty2 = cy - ch // 2
        tx3 = cx + cw // 2
        ty3 = cy + ch // 2
        tx4 = cx - cw // 2
        ty4 = cy + ch // 2
        target_pts = np.array([[tx1, ty1], [tx2, ty2], [tx3, ty3], [tx4, ty4]]).astype(np.float32) * scale
        org_pts = np.array([[x1, y1], [x2, y2], [x3, y3], [x4, y4]]).astype(np.float32)
        mat_ = cv2.estimateRigidTransform(org_pts, target_pts, True)
        dsize = (int(120 * scale), int(48 * scale))
        warped = cv2.warpAffine(image, mat_, dsize)
        return warped

    def affine_crop(self, image, pts):
        """
        crop a image by affine transform.
        :param image_rgb: input image
        :param pts: points
        """
        x1, y1, x2, y2, x3, y3, x4, y4 = pts.ravel()
        target_pts = np.array([[0, 0], [136, 0], [136, 36], [0, 36]]).astype(np.float32)
        org_pts = np.array([[x1, y1], [x2, y2], [x3, y3], [x4, y4]]).astype(np.float32)
        mat = cv2.getPerspectiveTransform(org_pts, target_pts)
        dsize = (136, 36)
        warped = cv2.warpPerspective(image, mat, dsize)
        return warped

    def finetune(self,image_, stage=2):
        """
        cascade fine tune a image by regress four corner of plate.
        :param image_rgb: input image
        :param stages: cascade stage
        """

        tof = image_.copy()
        image = cv2.resize(tof, (120, 48))
        blob = cv2.dnn.blobFromImage(image, size=(120, 48), swapRB=False, mean=(127.5, 127.5, 127.5), scalefactor=0.0078125, crop=False)
        self.refine_net.setInput(blob)
        h, w, c = image_.shape
        pts = (self.refine_net.forward("conv6-3").reshape(4, 2) * np.array([w, h])).astype(np.int)
        g = self.to_refine(image_, pts)
        blob = cv2.dnn.blobFromImage(g, size=(120, 48), swapRB=False, mean=(127.5, 127.5, 127.5), scalefactor=0.0078125, crop=False)
        self.refine_net.setInput(blob)
        h, w, c = g.shape
        pts = (self.refine_net.forward("conv6-3").reshape(4, 2) * np.array([w, h])).astype(np.int)
        cropped = self.affine_crop(g, pts)
        return cropped


    def segmentation_free_recognition(self,src):
        """
        return: ctc decode results
        """
        temp = cv2.resize(src,( 160,40))
        temp = temp.transpose(1, 0, 2)
        blob = cv2.dnn.blobFromImage(temp, 1/255.0, (40, 160), (0,0,0), False, False)
        self.modelRecognition.setInput(blob)
        y_pred = self.modelRecognition.forward()[0]
        y_pred = y_pred[:,2:,:]
        y_pred = np.squeeze(y_pred)
        return self.decode_ctc(y_pred)


    def plate_recognition(self,image,minSize=30,charSelectionDeskew=True,DB = True, mode='ssd'):
        """
        the simple pipline consists of detection . deskew , fine mapping alignment, recognition.
        :param image: the input BGR image from imread used by opencv
        :param minSize: the minSize of plate
        :param charSelectionDeskew: use character detection when fine mapping stage which will reduce the False Accept Rate as far as possible.
        :return: will return [ [plate1 string ,confidence1, location1  ],
                               [plate2 string ,confidence2, location2  ] ....
                             ]

        usage:
            import cv2
            import numpy as np
            from hyperlpr import LPR
            pr = LPR("models")
            image  = cv2.imread("tests/image")
            print(pr.plateRecognition(image))
        """
        if DB:
            image_gray = cv2.cvtColor(image,cv2.COLOR_BGR2GRAY)
            images = self.detect_traditional(image_gray)
        else:
            images = self.detect_ssd(image)
        res_set = []
        for j,plate in enumerate(images):
            plate,[left,top,right,bottom] = plate
            print(left, top, right, bottom)
            if DB:
                w, h = right - left, bottom - top
                plate = image[top:bottom,left:right,:]
                crop_up = plate[int(h * 0.05):int((h) * 0.4), int(w * 0.2):int(w * 0.75)]
                crop_down = plate[int((h) * 0.4):int(h), int(w * 0.05):w]
                crop_up = cv2.resize(crop_up, (64, 40))
                crop_down = cv2.resize(crop_down, (96, 40))
                cropped_finetuned = np.concatenate([crop_up, crop_down], 1)
                # cv2.imshow("crop",plate)
                # cv2.waitKey(0)
            else:

                cropped = self.loose_crop(image, [left, top, right, bottom], 120 / 48)
                cropped_finetuned = self.finetune(cropped)
            res, confidence = self.segmentation_free_recognition(cropped_finetuned)
            res_set.append([res,confidence,[left,top,right,bottom ]])
        return res_set


#
