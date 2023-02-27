package com.hyperai.hyperlpr_sdk_demo;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ArrayAdapter;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;


import com.hyperai.hyperlpr3.HyperLPR3;
import com.hyperai.hyperlpr3.bean.Plate;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.List;


/**
 * @author by hs-johnny
 * Created on 2019/6/17
 */
public class CameraActivity extends Activity {

    FrameLayout previewFl;
    CameraPreviews cameraPreview;
    TextView plateTv;
//    TextView regTv;
    ImageView image;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_camera);
    }

    private void initCamera() {
        previewFl = findViewById(R.id.preview_fl);
        plateTv = findViewById(R.id.plate_tv);
        image = findViewById(R.id.image);
        cameraPreview = new CameraPreviews(this);
        previewFl.addView(cameraPreview);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (cameraPreview == null) {
            initCamera();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        cameraPreview = null;
    }

    private void stopPreview() {
        previewFl.removeAllViews();
    }

    @Override
    protected void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }

    @Override
    protected void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }

    @SuppressLint("SetTextI18n")
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Plate[] plates) {

        String showText = "";
        for (Plate plate: plates) {
            String type = "未知车牌";
            if (plate.getType() != HyperLPR3.PLATE_TYPE_UNKNOWN) {
                type = HyperLPR3.PLATE_TYPE_MAPS[plate.getType()];
            }
            String pStr = "[" + type + "]" + plate.getCode() + "\n";
            showText += pStr;
            plateTv.setText(showText);

        }

//        stopPreview();
    }
}
