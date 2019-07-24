package pr.platerecognization;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.database.Cursor;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.DocumentsContract;
import android.provider.MediaStore;
import android.support.v4.app.ActivityCompat;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends Activity implements AlertDialog.OnClickListener,View.OnClickListener{

    // Used to load the 'native-lib' library on application startup.
    static {

        if(OpenCVLoader.initDebug())
        {
            Log.d("Opencv","opencv load_success");

        }
        else
        {
            Log.d("Opencv","opencv can't load opencv .");

        }
    }

    public Button btn;
    public Button recogBtn;
    public TextView resbox;
    public TextView runtimebox;

    public ImageView imgv;
    public SeekBar sb;

    private static final int REQUEST_CODE_IMAGE_CAMERA = 1;
    private static final int REQUEST_CODE_IMAGE_OP = 2;
    private static final int REQUEST_CODE_OP = 3;
    private Uri mPath;

    Bitmap latestBitmap;

//    public  PlateRecognition pr;

    public static long handle;
    private final String TAG = this.getClass().toString();



    public void copyFilesFromAssets(Context context, String oldPath, String newPath) {
        try {
            String[] fileNames = context.getAssets().list(oldPath);
            if (fileNames.length > 0) {
                // directory
                File file = new File(newPath);
                if (!file.mkdir())
                {
                    Log.d("mkdir","can't make folder");

                }
//                    return false;                // copy recursively
                for (String fileName : fileNames) {
                    copyFilesFromAssets(context, oldPath + "/" + fileName,
                            newPath + "/" + fileName);
                }
            } else {
                // file
                InputStream is = context.getAssets().open(oldPath);
                FileOutputStream fos = new FileOutputStream(new File(newPath));
                byte[] buffer = new byte[1024];
                int byteCount;
                while ((byteCount = is.read(buffer)) != -1) {
                    fos.write(buffer, 0, byteCount);
                }
                fos.flush();
                is.close();
                fos.close();
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }





    public void initRecognizer()
    {
        String assetPath = "pr";
        String sdcardPath = Environment.getExternalStorageDirectory()
                + File.separator + assetPath;
        copyFilesFromAssets(this, assetPath, sdcardPath);
        String cascade_filename  =  sdcardPath
            + File.separator+"cascade.xml";
        String finemapping_prototxt  =  sdcardPath
                + File.separator+"HorizonalFinemapping.prototxt";
        String finemapping_caffemodel  =  sdcardPath
                + File.separator+"HorizonalFinemapping.caffemodel";
        String segmentation_prototxt =  sdcardPath
                + File.separator+"Segmentation.prototxt";
        String segmentation_caffemodel =  sdcardPath
                + File.separator+"Segmentation.caffemodel";
        String character_prototxt =  sdcardPath
                + File.separator+"CharacterRecognization.prototxt";
        String character_caffemodel=  sdcardPath
                + File.separator+"CharacterRecognization.caffemodel";
        String segmentationfree_prototxt =  sdcardPath
                + File.separator+"SegmenationFree-Inception.prototxt";
        String segmentationfree_caffemodel=  sdcardPath
                + File.separator+"SegmenationFree-Inception.caffemodel";
        handle  =  PlateRecognition.InitPlateRecognizer(
                cascade_filename,
                finemapping_prototxt,finemapping_caffemodel,
                segmentation_prototxt,segmentation_caffemodel,
                character_prototxt,character_caffemodel,
                segmentationfree_prototxt,segmentationfree_caffemodel
        );


    }

    /**
     * Get the value of the data column for this Uri. This is useful for
     * MediaStore Uris, and other file-based ContentProviders.
     *
     * @param context The context.
     * @param uri The Uri to query.
     * @param selection (Optional) Filter used in the query.
     * @param selectionArgs (Optional) Selection arguments used in the query.
     * @return The value of the _data column, which is typically a file path.
     */
    public static String getDataColumn(Context context, Uri uri, String selection,
                                       String[] selectionArgs) {

        Cursor cursor = null;
        final String column = "_data";
        final String[] projection = {
                column
        };

        try {
            cursor = context.getContentResolver().query(uri, projection, selection, selectionArgs,
                    null);
            if (cursor != null && cursor.moveToFirst()) {
                final int index = cursor.getColumnIndexOrThrow(column);
                return cursor.getString(index);
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return null;
    }

    /**
     * @param uri
     * @return
     */
    private String getPath(Uri uri) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
            if (DocumentsContract.isDocumentUri(this, uri)) {
                final String docId = DocumentsContract.getDocumentId(uri);
                final String[] split = docId.split(":");
                final String type = split[0];

                Uri contentUri = null;
                if ("image".equals(type)) {
                    contentUri = MediaStore.Images.Media.EXTERNAL_CONTENT_URI;
                } else if ("video".equals(type)) {
                    contentUri = MediaStore.Video.Media.EXTERNAL_CONTENT_URI;
                    return null;
                } else if ("audio".equals(type)) {
                    contentUri = MediaStore.Audio.Media.EXTERNAL_CONTENT_URI;
                    return null;
                }

                final String selection = "_id=?";
                final String[] selectionArgs = new String[] {
                        split[1]
                };

                return getDataColumn(this, contentUri, selection, selectionArgs);
            }
        }
        String[] proj = { MediaStore.Images.Media.DATA };
        Cursor actualimagecursor = managedQuery(uri, proj,null,null,null);
        int actual_image_column_index = actualimagecursor.getColumnIndexOrThrow(MediaStore.Images.Media.DATA);
        actualimagecursor.moveToFirst();
        String img_path = actualimagecursor.getString(actual_image_column_index);
        String end = img_path.substring(img_path.length() - 4);
        if (0 != end.compareToIgnoreCase(".jpg") && 0 != end.compareToIgnoreCase(".png")) {
            return null;
        }
        return img_path;
    }

    public static Bitmap decodeImage(String path) {
        Bitmap res;
        try {
            ExifInterface exif = new ExifInterface(path);
            int orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL);

            BitmapFactory.Options op = new BitmapFactory.Options();
            op.inSampleSize = 1;
            op.inJustDecodeBounds = false;
            //op.inMutable = true;
            res = BitmapFactory.decodeFile(path, op);
            //rotate and scale.
            Matrix matrix = new Matrix();

            if (orientation == ExifInterface.ORIENTATION_ROTATE_90) {
                matrix.postRotate(90);
            } else if (orientation == ExifInterface.ORIENTATION_ROTATE_180) {
                matrix.postRotate(180);
            } else if (orientation == ExifInterface.ORIENTATION_ROTATE_270) {
                matrix.postRotate(270);
            }

            Bitmap temp = Bitmap.createBitmap(res, 0, 0, res.getWidth(), res.getHeight(), matrix, true);
            Log.d("com.arcsoft", "check target Image:" + temp.getWidth() + "X" + temp.getHeight());

            if (!temp.equals(res)) {
                res.recycle();
            }
            return temp;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    public void SimpleRecog(Bitmap bmp,int dp)
    {

        float dp_asp  = dp/10.f;
//        imgv.setImageBitmap(bmp);
        Mat mat_src = new Mat(bmp.getWidth(), bmp.getHeight(), CvType.CV_8UC4);

        float new_w = bmp.getWidth()*dp_asp;
        float new_h = bmp.getHeight()*dp_asp;
        Size sz = new Size(new_w,new_h);
        Utils.bitmapToMat(bmp, mat_src);
        Imgproc.resize(mat_src,mat_src,sz);
        long currentTime1 = System.currentTimeMillis();
//        String res = PlateRecognition.SimpleRecognization(mat_src.getNativeObjAddr(),handle);
//        resbox.setText("识别结果:"+res);

        PlateInfo plateInfo = PlateRecognition.PlateInfoRecognization(mat_src.getNativeObjAddr(),handle);
        imgv.setImageBitmap(plateInfo.bitmap);
        resbox.setText("识别结果:"+plateInfo.plateName);
        long diff = System.currentTimeMillis() - currentTime1;

        runtimebox.setText(String.valueOf(diff)+"ms");



    }

    public  static PlateInfo simpleRecog(Bitmap bmp,int dp)
    {

        float dp_asp  = dp/10.f;
//        imgv.setImageBitmap(bmp);
        Mat mat_src = new Mat(bmp.getWidth(), bmp.getHeight(), CvType.CV_8UC4);

        float new_w = bmp.getWidth()*dp_asp;
        float new_h = bmp.getHeight()*dp_asp;
        Size sz = new Size(new_w,new_h);
        Utils.bitmapToMat(bmp, mat_src);
        Imgproc.resize(mat_src,mat_src,sz);
        long currentTime1 = System.currentTimeMillis();
//        String res = PlateRecognition.SimpleRecognization(mat_src.getNativeObjAddr(),handle);
//        resbox.setText("识别结果:"+res);

        PlateInfo plateInfo = PlateRecognition.PlateInfoRecognization(mat_src.getNativeObjAddr(),handle);
        return plateInfo;

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_CODE_IMAGE_OP && resultCode == RESULT_OK) {
            mPath = data.getData();
            String file = getPath(mPath);
            Bitmap bmp = decodeImage(file);
            if (bmp == null || bmp.getWidth() <= 0 || bmp.getHeight() <= 0 ) {
                Log.e(TAG, "error");
            } else {
                Log.i(TAG, "bmp [" + bmp.getWidth() + "," + bmp.getHeight());
            }
            latestBitmap = bmp;

            SimpleRecog(bmp,sb.getProgress());
        } else if (requestCode == REQUEST_CODE_OP) {
            Log.i(TAG, "RESULT =" + resultCode);
            if (data == null) {
                return;
            }
            Bundle bundle = data.getExtras();
            String path = bundle.getString("imagePath");
            Log.i(TAG, "path="+path);
        } else if (requestCode == REQUEST_CODE_IMAGE_CAMERA && resultCode == RESULT_OK) {
            String file = getPath(mPath);
            Bitmap bmp = decodeImage(file);
            latestBitmap = bmp;
            SimpleRecog(bmp,sb.getProgress());
//            startOilPainting(bmp, file);
        }

    }



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String permission = Manifest.permission.WRITE_EXTERNAL_STORAGE;
        if (ActivityCompat.checkSelfPermission(MainActivity.this, permission)
                != PackageManager.PERMISSION_GRANTED
                ) {
            ActivityCompat.requestPermissions(MainActivity.this, new String[]
                    {permission},123);

        }
        btn = (Button)findViewById(R.id.button);
        recogBtn = (Button)findViewById(R.id.button_recog);
        findViewById(R.id.button_recog_now).setOnClickListener(this);
        imgv = (ImageView)findViewById(R.id.imageView);
        resbox = (TextView)findViewById(R.id.textView);
        sb = (SeekBar)findViewById(R.id.seek);
        runtimebox = (TextView)findViewById(R.id.textView3);


        initRecognizer();
        btn.setOnClickListener(this);
        recogBtn.setOnClickListener(this);

    }

    @Override
    public void onClick(DialogInterface dialogInterface, int which) {
        switch (which){
            case 1:
                Intent getImageByCamera = new Intent(
                        "android.media.action.IMAGE_CAPTURE");
                ContentValues values = new ContentValues(1);

                values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
                mPath = getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
                getImageByCamera.putExtra(MediaStore.EXTRA_OUTPUT, mPath);

                startActivityForResult(getImageByCamera, REQUEST_CODE_IMAGE_CAMERA);
                break;
            case 0:
                Intent getImageByalbum = new Intent(Intent.ACTION_GET_CONTENT);
                getImageByalbum.addCategory(Intent.CATEGORY_OPENABLE);
                getImageByalbum.setType("image/jpeg");
                startActivityForResult(getImageByalbum, REQUEST_CODE_IMAGE_OP);
                break;
            default:;
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {

            case R.id.button:
                new AlertDialog.Builder(this)
                        .setTitle("打开方式")
                        .setItems(new String[]{"打开图片"}, this)
                        .show();
                break;
            case R.id.button_recog:
               if(latestBitmap!=null){
                   SimpleRecog(latestBitmap,sb.getProgress());
               }
                break;
            case R.id.button_recog_now:
                Intent intent = new Intent(this, CameraActivity.class);
                startActivity(intent);
                break;
            default:
                break;
        }
    }
}

