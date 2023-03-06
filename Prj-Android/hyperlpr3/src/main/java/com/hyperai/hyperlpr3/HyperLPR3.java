package com.hyperai.hyperlpr3;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import com.hyperai.hyperlpr3.api.APIDefine;
import com.hyperai.hyperlpr3.bean.HyperLPRParameter;
import com.hyperai.hyperlpr3.bean.Plate;
import com.hyperai.hyperlpr3.core.HyperLPRCore;
import com.hyperai.hyperlpr3.settings.TypeDefine;
import com.hyperai.hyperlpr3.settings.SDKConfig;
import com.hyperai.hyperlpr3.utils.SDKUtils;

public class HyperLPR3 extends TypeDefine implements APIDefine {

    private final String TAG = "HyperLPR3-SDK";

    private final HyperLPRCore mCore;

    private boolean isInitSuccess;

    private HyperLPR3() {
        mCore = new HyperLPRCore();
        isInitSuccess = false;
    }

    private static class LazyHolder {
        private static final HyperLPR3 INSTANCE = new HyperLPR3();
    }

    public static final HyperLPR3 getInstance() {
        return LazyHolder.INSTANCE;
    }


    public void release() {
        mCore.release();
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    /**
     * Initialize the license plate recognition algorithm SDK
     *
     * @param context   context
     * @param parameter Initialization parameter
     */
    @Override
    public void init(Context context, HyperLPRParameter parameter) {
        if (!isInitSuccess) {
            String mResourceFolderPath = context.getExternalFilesDir(null).getAbsolutePath() + "/";
            SDKUtils.copyFilesFromAssets(context, SDKConfig.packDirName, mResourceFolderPath);
            Log.i(TAG, "resource: " + mResourceFolderPath);
            if (parameter.getModelPath() == null || "".equals(parameter.getModelPath())) {
                parameter.setModelPath(mResourceFolderPath);
            }
            mCore.createRecognizerContext(parameter);
            isInitSuccess = true;
        }
    }

    /**
     * License plate recognition interface.
     *
     * @param buf      Image data buffer.
     * @param height   Height of the image
     * @param width    Width of the image
     * @param rotation Original data buffer rotation Angle
     * @param format   Buffer data coded format
     * @return Resulting object array
     */
    @Override
    public Plate[] plateRecognition(byte[] buf, int height, int width, int rotation, int format) {
        if (!isInitSuccess) {
            Log.e(TAG, "HyperLPR3 is uninitialized.");
            return new Plate[0];
        }
        return mCore.plateRecognitionFromBuffer(buf, height, width, rotation, format);
    }

    /**
     * License plate recognition interface.
     *
     * @param image    Bitmap image
     * @param rotation Original data buffer rotation Angle
     * @param format   Buffer data coded format
     * @return Resulting object array
     */
    @Override
    public Plate[] plateRecognition(Bitmap image, int rotation, int format) {
        if (!isInitSuccess) {
            Log.e(TAG, "HyperLPR3 is uninitialized.");
            return new Plate[0];
        }
        int mWidth = image.getWidth();
        int mHeight = image.getHeight();
        int[] data = new int[image.getWidth() * image.getHeight()];
        image.getPixels(data, 0, mWidth, 0, 0, mWidth, mHeight);
        return mCore.plateRecognitionFromImage(data, mHeight, mWidth, rotation, format);
    }


}
