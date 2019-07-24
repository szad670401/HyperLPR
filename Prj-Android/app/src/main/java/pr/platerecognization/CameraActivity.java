package pr.platerecognization;

import android.app.Activity;
import android.media.Image;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;


/**
 * @author by hs-johnny
 * Created on 2019/6/17
 */
public class CameraActivity extends Activity implements View.OnClickListener {

    FrameLayout previewFl;
    CameraPreviews cameraPreview;
    TextView plateTv;
    TextView regTv;
    ImageView image;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_camera);
    }

    private void initCamera(){
        previewFl = findViewById(R.id.preview_fl);
        plateTv = findViewById(R.id.plate_tv);
        regTv = findViewById(R.id.reg_tv);
        regTv.setOnClickListener(this);
        image = findViewById(R.id.image);
        cameraPreview = new CameraPreviews(this);
        previewFl.addView(cameraPreview);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if(cameraPreview == null){
            initCamera();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        cameraPreview = null;
    }

    private void stopPreview(){
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

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(PlateInfo plate){
        plateTv.setText(plate.plateName);
        image.setImageBitmap(plate.bitmap);
        stopPreview();
    }

    @Override
    public void onClick(View v) {
        CameraPreviews cameraPreview = new CameraPreviews(this);
        previewFl.addView(cameraPreview);
    }
}
