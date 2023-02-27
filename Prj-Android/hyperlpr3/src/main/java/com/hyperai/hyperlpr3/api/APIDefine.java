package com.hyperai.hyperlpr3.api;

import com.hyperai.hyperlpr3.bean.Plate;

public interface APIDefine {

    /**
     * License plate recognition interface.
     * @param buf Image data buffer.
     * @param height Height of the image
     * @param width Width of the image
     * @param rotation Original data buffer rotation Angle
     * @param format Buffer data coded format
     * @return Resulting object array
     */
    Plate[] plateRecognition(byte[] buf, int height, int width, int rotation, int format);

}
