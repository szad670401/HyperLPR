//
// This file is auto-generated. Please don't modify it!
//
package org.opencv.dnn;

import java.lang.String;
import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Mat;
import org.opencv.core.MatOfFloat;
import org.opencv.core.MatOfInt;
import org.opencv.core.MatOfRect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.dnn.Net;
import org.opencv.utils.Converters;

// C++: class Dnn
//javadoc: Dnn

public class Dnn {

    public static final int
            DNN_BACKEND_DEFAULT = 0,
            DNN_BACKEND_HALIDE = 1,
            DNN_BACKEND_INFERENCE_ENGINE = 2,
            DNN_BACKEND_OPENCV = 3,
            DNN_TARGET_CPU = 0,
            DNN_TARGET_OPENCL = 1,
            DNN_TARGET_OPENCL_FP16 = 2,
            DNN_TARGET_MYRIAD = 3;


    //
    // C++:  Mat blobFromImage(Mat image, double scalefactor = 1.0, Size size = Size(), Scalar mean = Scalar(), bool swapRB = true, bool crop = true)
    //

    //javadoc: blobFromImage(image, scalefactor, size, mean, swapRB, crop)
    public static Mat blobFromImage(Mat image, double scalefactor, Size size, Scalar mean, boolean swapRB, boolean crop)
    {
        
        Mat retVal = new Mat(blobFromImage_0(image.nativeObj, scalefactor, size.width, size.height, mean.val[0], mean.val[1], mean.val[2], mean.val[3], swapRB, crop));
        
        return retVal;
    }

    //javadoc: blobFromImage(image)
    public static Mat blobFromImage(Mat image)
    {
        
        Mat retVal = new Mat(blobFromImage_1(image.nativeObj));
        
        return retVal;
    }


    //
    // C++:  Mat blobFromImages(vector_Mat images, double scalefactor = 1.0, Size size = Size(), Scalar mean = Scalar(), bool swapRB = true, bool crop = true)
    //

    //javadoc: blobFromImages(images, scalefactor, size, mean, swapRB, crop)
    public static Mat blobFromImages(List<Mat> images, double scalefactor, Size size, Scalar mean, boolean swapRB, boolean crop)
    {
        Mat images_mat = Converters.vector_Mat_to_Mat(images);
        Mat retVal = new Mat(blobFromImages_0(images_mat.nativeObj, scalefactor, size.width, size.height, mean.val[0], mean.val[1], mean.val[2], mean.val[3], swapRB, crop));
        
        return retVal;
    }

    //javadoc: blobFromImages(images)
    public static Mat blobFromImages(List<Mat> images)
    {
        Mat images_mat = Converters.vector_Mat_to_Mat(images);
        Mat retVal = new Mat(blobFromImages_1(images_mat.nativeObj));
        
        return retVal;
    }


    //
    // C++:  Mat readTorchBlob(String filename, bool isBinary = true)
    //

    //javadoc: readTorchBlob(filename, isBinary)
    public static Mat readTorchBlob(String filename, boolean isBinary)
    {
        
        Mat retVal = new Mat(readTorchBlob_0(filename, isBinary));
        
        return retVal;
    }

    //javadoc: readTorchBlob(filename)
    public static Mat readTorchBlob(String filename)
    {
        
        Mat retVal = new Mat(readTorchBlob_1(filename));
        
        return retVal;
    }


    //
    // C++:  Net readNet(String model, String config = "", String framework = "")
    //

    //javadoc: readNet(model, config, framework)
    public static Net readNet(String model, String config, String framework)
    {
        
        Net retVal = new Net(readNet_0(model, config, framework));
        
        return retVal;
    }

    //javadoc: readNet(model)
    public static Net readNet(String model)
    {
        
        Net retVal = new Net(readNet_1(model));
        
        return retVal;
    }


    //
    // C++:  Net readNetFromCaffe(String prototxt, String caffeModel = String())
    //

    //javadoc: readNetFromCaffe(prototxt, caffeModel)
    public static Net readNetFromCaffe(String prototxt, String caffeModel)
    {
        
        Net retVal = new Net(readNetFromCaffe_0(prototxt, caffeModel));
        
        return retVal;
    }

    //javadoc: readNetFromCaffe(prototxt)
    public static Net readNetFromCaffe(String prototxt)
    {
        
        Net retVal = new Net(readNetFromCaffe_1(prototxt));
        
        return retVal;
    }


    //
    // C++:  Net readNetFromDarknet(String cfgFile, String darknetModel = String())
    //

    //javadoc: readNetFromDarknet(cfgFile, darknetModel)
    public static Net readNetFromDarknet(String cfgFile, String darknetModel)
    {
        
        Net retVal = new Net(readNetFromDarknet_0(cfgFile, darknetModel));
        
        return retVal;
    }

    //javadoc: readNetFromDarknet(cfgFile)
    public static Net readNetFromDarknet(String cfgFile)
    {
        
        Net retVal = new Net(readNetFromDarknet_1(cfgFile));
        
        return retVal;
    }


    //
    // C++:  Net readNetFromModelOptimizer(String xml, String bin)
    //

    //javadoc: readNetFromModelOptimizer(xml, bin)
    public static Net readNetFromModelOptimizer(String xml, String bin)
    {
        
        Net retVal = new Net(readNetFromModelOptimizer_0(xml, bin));
        
        return retVal;
    }


    //
    // C++:  Net readNetFromTensorflow(String model, String config = String())
    //

    //javadoc: readNetFromTensorflow(model, config)
    public static Net readNetFromTensorflow(String model, String config)
    {
        
        Net retVal = new Net(readNetFromTensorflow_0(model, config));
        
        return retVal;
    }

    //javadoc: readNetFromTensorflow(model)
    public static Net readNetFromTensorflow(String model)
    {
        
        Net retVal = new Net(readNetFromTensorflow_1(model));
        
        return retVal;
    }


    //
    // C++:  Net readNetFromTorch(String model, bool isBinary = true)
    //

    //javadoc: readNetFromTorch(model, isBinary)
    public static Net readNetFromTorch(String model, boolean isBinary)
    {
        
        Net retVal = new Net(readNetFromTorch_0(model, isBinary));
        
        return retVal;
    }

    //javadoc: readNetFromTorch(model)
    public static Net readNetFromTorch(String model)
    {
        
        Net retVal = new Net(readNetFromTorch_1(model));
        
        return retVal;
    }


    //
    // C++:  void NMSBoxes(vector_Rect bboxes, vector_float scores, float score_threshold, float nms_threshold, vector_int& indices, float eta = 1.f, int top_k = 0)
    //

    //javadoc: NMSBoxes(bboxes, scores, score_threshold, nms_threshold, indices, eta, top_k)
    public static void NMSBoxes(MatOfRect bboxes, MatOfFloat scores, float score_threshold, float nms_threshold, MatOfInt indices, float eta, int top_k)
    {
        Mat bboxes_mat = bboxes;
        Mat scores_mat = scores;
        Mat indices_mat = indices;
        NMSBoxes_0(bboxes_mat.nativeObj, scores_mat.nativeObj, score_threshold, nms_threshold, indices_mat.nativeObj, eta, top_k);
        
        return;
    }

    //javadoc: NMSBoxes(bboxes, scores, score_threshold, nms_threshold, indices)
    public static void NMSBoxes(MatOfRect bboxes, MatOfFloat scores, float score_threshold, float nms_threshold, MatOfInt indices)
    {
        Mat bboxes_mat = bboxes;
        Mat scores_mat = scores;
        Mat indices_mat = indices;
        NMSBoxes_1(bboxes_mat.nativeObj, scores_mat.nativeObj, score_threshold, nms_threshold, indices_mat.nativeObj);
        
        return;
    }


    //
    // C++:  void imagesFromBlob(Mat blob_, vector_Mat& images_)
    //

    //javadoc: imagesFromBlob(blob_, images_)
    public static void imagesFromBlob(Mat blob_, List<Mat> images_)
    {
        Mat images__mat = new Mat();
        imagesFromBlob_0(blob_.nativeObj, images__mat.nativeObj);
        Converters.Mat_to_vector_Mat(images__mat, images_);
        images__mat.release();
        return;
    }


    //
    // C++:  void shrinkCaffeModel(String src, String dst, vector_String layersTypes = std::vector<String>())
    //

    //javadoc: shrinkCaffeModel(src, dst, layersTypes)
    public static void shrinkCaffeModel(String src, String dst, List<String> layersTypes)
    {
        
        shrinkCaffeModel_0(src, dst, layersTypes);
        
        return;
    }

    //javadoc: shrinkCaffeModel(src, dst)
    public static void shrinkCaffeModel(String src, String dst)
    {
        
        shrinkCaffeModel_1(src, dst);
        
        return;
    }




    // C++:  Mat blobFromImage(Mat image, double scalefactor = 1.0, Size size = Size(), Scalar mean = Scalar(), bool swapRB = true, bool crop = true)
    private static native long blobFromImage_0(long image_nativeObj, double scalefactor, double size_width, double size_height, double mean_val0, double mean_val1, double mean_val2, double mean_val3, boolean swapRB, boolean crop);
    private static native long blobFromImage_1(long image_nativeObj);

    // C++:  Mat blobFromImages(vector_Mat images, double scalefactor = 1.0, Size size = Size(), Scalar mean = Scalar(), bool swapRB = true, bool crop = true)
    private static native long blobFromImages_0(long images_mat_nativeObj, double scalefactor, double size_width, double size_height, double mean_val0, double mean_val1, double mean_val2, double mean_val3, boolean swapRB, boolean crop);
    private static native long blobFromImages_1(long images_mat_nativeObj);

    // C++:  Mat readTorchBlob(String filename, bool isBinary = true)
    private static native long readTorchBlob_0(String filename, boolean isBinary);
    private static native long readTorchBlob_1(String filename);

    // C++:  Net readNet(String model, String config = "", String framework = "")
    private static native long readNet_0(String model, String config, String framework);
    private static native long readNet_1(String model);

    // C++:  Net readNetFromCaffe(String prototxt, String caffeModel = String())
    private static native long readNetFromCaffe_0(String prototxt, String caffeModel);
    private static native long readNetFromCaffe_1(String prototxt);

    // C++:  Net readNetFromDarknet(String cfgFile, String darknetModel = String())
    private static native long readNetFromDarknet_0(String cfgFile, String darknetModel);
    private static native long readNetFromDarknet_1(String cfgFile);

    // C++:  Net readNetFromModelOptimizer(String xml, String bin)
    private static native long readNetFromModelOptimizer_0(String xml, String bin);

    // C++:  Net readNetFromTensorflow(String model, String config = String())
    private static native long readNetFromTensorflow_0(String model, String config);
    private static native long readNetFromTensorflow_1(String model);

    // C++:  Net readNetFromTorch(String model, bool isBinary = true)
    private static native long readNetFromTorch_0(String model, boolean isBinary);
    private static native long readNetFromTorch_1(String model);

    // C++:  void NMSBoxes(vector_Rect bboxes, vector_float scores, float score_threshold, float nms_threshold, vector_int& indices, float eta = 1.f, int top_k = 0)
    private static native void NMSBoxes_0(long bboxes_mat_nativeObj, long scores_mat_nativeObj, float score_threshold, float nms_threshold, long indices_mat_nativeObj, float eta, int top_k);
    private static native void NMSBoxes_1(long bboxes_mat_nativeObj, long scores_mat_nativeObj, float score_threshold, float nms_threshold, long indices_mat_nativeObj);

    // C++:  void imagesFromBlob(Mat blob_, vector_Mat& images_)
    private static native void imagesFromBlob_0(long blob__nativeObj, long images__mat_nativeObj);

    // C++:  void shrinkCaffeModel(String src, String dst, vector_String layersTypes = std::vector<String>())
    private static native void shrinkCaffeModel_0(String src, String dst, List<String> layersTypes);
    private static native void shrinkCaffeModel_1(String src, String dst);

}
