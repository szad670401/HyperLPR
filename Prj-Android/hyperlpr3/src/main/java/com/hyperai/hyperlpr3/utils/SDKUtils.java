package com.hyperai.hyperlpr3.utils;

import android.content.Context;
import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.DashPathEffect;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class SDKUtils {
    static public void copyFilesFromAssets(Context context, String oldPath, String newPath) {
        try {
            String[] fileNames = context.getAssets().list(oldPath);
            if (fileNames.length > 0) {
                // directory
                File file = new File(newPath);
                if (!file.mkdir()) {
                    Log.d("mkdir", "can't make folder");
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

    static public void mkdirsFeaturesAssets(Context context, String path, String images, String features) {
        try {
            File file = new File(path);
            File imagePath = new File(path + images);
            File featurePath = new File(path + features);
            if (!file.mkdir()) {
                Log.d("mkdir", "can't make folder: " + file);
            }
            if (!imagePath.mkdir()) {
                Log.d("mkdir", "can't make folder: " + imagePath);
            }
            if (!featurePath.mkdir()) {
                Log.d("mkdir", "can't make folder: " + featurePath);
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    static public String saveBitmap(String savePath, String name, Bitmap bitmap) throws IOException {
        File f = new File(savePath, name + ".png");
        if (f.exists()) {
            f.delete();
        }
        FileOutputStream out = new FileOutputStream(f);
        bitmap.compress(Bitmap.CompressFormat.PNG, 100, out);
        out.flush();
        out.close();
        return f.getAbsolutePath();
    }


    static public boolean cropperBitmap(Bitmap bitmap, int[] rePoints, String sdcardPath, String name) {
        int newWidth = 160;
        int newHeight = 192;
        float scaleWidth = ((float) newWidth) / bitmap.getWidth();
        float scaleHeight = ((float) newHeight) / bitmap.getHeight();
        Matrix matrix = new Matrix();
//                                            matrix.postTranslate()
        matrix.postScale(scaleWidth, scaleHeight);
        matrix.setScale(-1, 1);
        int px = Math.max(0, rePoints[0]);
        int py = Math.max(0, rePoints[1]);
        int w = rePoints[2] - px;
        int h = rePoints[3] - py;
        int pw = (bitmap.getWidth() - w > 0) ? w - 1 : bitmap.getWidth() - 1;
        int ph = (bitmap.getHeight() - h > 0) ? h - 1 : bitmap.getHeight() - 1;
        try {
            Bitmap crop = Bitmap.createBitmap(bitmap, px, py, pw, ph, matrix, true);
//            String sdcardPath =  Environment.getExternalStorageDirectory() + File.separator + Const.root + Const.save + Const.images;
            saveBitmap(sdcardPath, name, crop);
        } catch (Exception err) {
            Log.d("crop", "ext");
            return false;
        }
        return true;
    }


    public static Bitmap getImageFromAssetsFile(Context context, String fileName) {
        Bitmap image = null;
        AssetManager am = context.getResources().getAssets();
        try {
            InputStream is = am.open(fileName);
            image = BitmapFactory.decodeStream(is);
            is.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return image;
    }



}
