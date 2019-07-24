package pr.platerecognization;

import android.graphics.Bitmap;

import org.opencv.core.Mat;

/**
 * @author liuxuhui
 * @date 2019/6/20
 */
public class PlateInfo {

    /**
     * 车牌号
     */
    public String plateName;

    /**
     * 车牌号图片
     */
    public Bitmap bitmap;

    public PlateInfo() {
    }

    public PlateInfo(String plateName, Bitmap bitmap) {
        this.plateName = plateName;
        this.bitmap = bitmap;
    }
}
