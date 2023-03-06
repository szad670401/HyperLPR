package com.hyperai.hyperlpr3.core;

import android.util.Log;

import com.hyperai.hyperlpr3.bean.HyperLPRParameter;
import com.hyperai.hyperlpr3.bean.Plate;

public class HyperLPRCore {

    final String TAG = "HyperLPRCore";

    static {
        System.loadLibrary("hyperlpr3");
    }

    private long ctxHandle_;

    private boolean isRunning_;

    public void createRecognizerContext(HyperLPRParameter parameter) {
        ctxHandle_ = CreateRecognizerContext(parameter);
        Log.i(TAG, "HANDLE: " + ctxHandle_);
        isRunning_ = true;
    }

    public Plate[] plateRecognitionFromBuffer(byte[] buf, int height, int width, int rotation, int format) {
        return PlateRecognitionFromBuffer(ctxHandle_, buf, height, width, rotation, format);
    }

    public Plate[] plateRecognitionFromImage(int[] buf, int height, int width, int rotation, int format) {
        return PlateRecognitionFromImage(ctxHandle_, buf, height, width, rotation, format);
    }

    public int release() {
        int ret = -1;
        if (isRunning_) {
            ret = ReleaseRecognizerContext(ctxHandle_);
            isRunning_ = false;
            ctxHandle_ = 0;
        }
        return ret;
    }

    // ===================Native==================

//    native void TestBuffer(String savePath, byte[] buf, int height, int width, int rotation);

    native long CreateRecognizerContext(HyperLPRParameter parameterObj);

    native int ReleaseRecognizerContext(long handle);

    native Plate[] PlateRecognitionFromBuffer(long handle, byte[] buf, int height, int width, int rotation, int format);

    native Plate[] PlateRecognitionFromImage(long handle, int[] buf, int height, int width, int rotation, int format);


}
