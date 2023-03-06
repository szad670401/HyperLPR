package com.hyperai.hyperlpr_sdk_demo;

import android.content.Context;
import android.graphics.Paint;
import android.hardware.Camera;
import android.os.Handler;
import android.os.HandlerThread;
//import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;


import org.greenrobot.eventbus.EventBus;

import java.io.IOException;
import java.util.List;

import com.hyperai.hyperlpr3.HyperLPR3;
import com.hyperai.hyperlpr3.bean.HyperLPRParameter;
import com.hyperai.hyperlpr3.bean.Plate;

/**
 * @author by hs-johnny
 * Created on 2019/6/17
 */
public class CameraPreviews extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback {

    private static final String TAG = "CameraPreview";
    private Camera mCamera;
    private SurfaceHolder mHolder;
    public long handle;
    private byte[] lock = new byte[0];
    private Paint mPaint;
    private float oldDist = 1f;
    /** 停止识别*/
    private boolean isStopReg;

    private Context mContext;

    public CameraPreviews(Context context) {
        super(context);
        mContext = context;
        mHolder = getHolder();
        mHolder.addCallback(this);
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
        mPaint.setStrokeWidth(2);
        mPaint.setStyle(Paint.Style.STROKE);
//        mPaint.setColor(ContextCompat.getColor(context, R.color.colorAccent));

        HyperLPRParameter parameter = new HyperLPRParameter();
//        hyperLPR3 = new HyperLPR3();
//        hyperLPR3.init(mContext, parameter);
    }
    public Camera getCameraInstance(){
        if (mCamera == null){
            try {
                CameraHandlerThread mThread = new CameraHandlerThread("camera thread");
                synchronized (mThread){
                    mThread.openCamera();
                }
            }catch (Exception e){
                Log.e(TAG, "camera is not available" );
            }
        }
        return mCamera;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mCamera = getCameraInstance();
        mCamera.setPreviewCallback(this);
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.startPreview();
            setPreviewFocus(mCamera);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        int rotation = getDisplayOrientation();
        mCamera.setDisplayOrientation(rotation);
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setRotation(rotation);
//        parameters.setPreviewFormat(ImageFormat.NV21);
        mCamera.setParameters(parameters);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mHolder.removeCallback(this);
        mCamera.setPreviewCallback(null);
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }

    public int getDisplayOrientation(){
        Display display = ((WindowManager)getContext().getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int rotation = display.getRotation();
        int degrees = 0;
        switch (rotation){
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(Camera.CameraInfo.CAMERA_FACING_BACK, info);
        int result = (info.orientation - degrees + 360) % 360;
        return result;
    }

    @Override
    public void onPreviewFrame(final byte[] data, final Camera camera) {
        synchronized (lock){
            // 处理data
            Camera.Size previewSize = camera.getParameters().getPreviewSize();
            Plate[] plates = HyperLPR3.getInstance().plateRecognition(data, previewSize.height, previewSize.width, HyperLPR3.CAMERA_ROTATION_270, HyperLPR3.STREAM_YUV_NV21);
            for (Plate plate : plates) {
                Log.i(TAG,  "" + plate.toString());
            }

            if(!isStopReg && plates.length > 0) {
//                isStopReg = true;
                sendPlate(plates);
            }

        }
    }

    private void sendPlate(Plate[] plates){
        EventBus.getDefault().post(plates);
    }


    private void openCameraOriginal(){
        try {
            mCamera = Camera.open();
        }catch (Exception e){
            Log.e(TAG, "camera is not available");
        }
    }

    private class CameraHandlerThread extends HandlerThread {
        Handler handler;
        public CameraHandlerThread(String name) {
            super(name);
            start();
            handler = new Handler(getLooper());
        }

        synchronized void notifyCameraOpened(){
            notify();
        }

        void openCamera(){
            handler.post(new Runnable() {
                @Override
                public void run() {
                    openCameraOriginal();
                    notifyCameraOpened();
                }
            });
            try {
                wait();
            }catch (Exception e){
                Log.e(TAG, "wait was interrupted");
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if(event.getPointerCount() == 2){
            switch (event.getAction()){
                case MotionEvent.ACTION_POINTER_DOWN:
                    oldDist = getFingerSpacing(event);
                    break;
                case MotionEvent.ACTION_MOVE:
                    float newDist = getFingerSpacing(event);
                    if(newDist > oldDist){
                        handleZoom(true, mCamera);
                    }else if(newDist < oldDist){
                        handleZoom(false, mCamera);
                    }
                    oldDist = newDist;
                    break;
            }
        }
        return true;
    }

    private float getFingerSpacing(MotionEvent event){
        float x = event.getX(0) - event.getX(1);
        float y = event.getY(0) - event.getY(1);
        return (float) Math.sqrt(x * x + y * y);
    }

    private void handleZoom(boolean isZoomIn, Camera camera){
        Camera.Parameters parameters = camera.getParameters();
        if (parameters.isZoomSupported()){
            int maxZoom = parameters.getMaxZoom();
            int zoom = parameters.getZoom();
            if (isZoomIn && zoom < maxZoom){
                zoom++;
            }else if(zoom > 0){
                zoom--;
            }
            parameters.setZoom(zoom);
            camera.setParameters(parameters);
        }else {
            Log.e(TAG, "handleZoom: "+"the device is not support zoom");
        }
    }

    private void setPreviewFocus(Camera camera){
        Camera.Parameters parameters = camera.getParameters();
        List<String> focusList = parameters.getSupportedFocusModes();
        if(focusList.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)){
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }
        camera.setParameters(parameters);
    }
}
