package com.hyperai.hyperlpr3.bean;

import com.hyperai.hyperlpr3.settings.TypeDefine;

public class Parameter {

    private String modelPath;

    private int threads = 1;

    private boolean useHalf = true;

    private float boxConfThreshold = 0.25f;

    private float nmsThreshold = 0.6f;

    private float recConfidenceThreshold = 0.75f;

    private int detLevel = TypeDefine.DETECT_LEVEL_LOW;

    private int maxNum = 3;

    public Parameter() {
    }

    public int getMaxNum() {
        return maxNum;
    }

    public void setMaxNum(int maxNum) {
        this.maxNum = maxNum;
    }

    public String getModelPath() {
        return modelPath;
    }

    public void setModelPath(String modelPath) {
        this.modelPath = modelPath;
    }

    public int getThreads() {
        return threads;
    }

    public void setThreads(int threads) {
        this.threads = threads;
    }

    public boolean isUseHalf() {
        return useHalf;
    }

    public void setUseHalf(boolean useHalf) {
        this.useHalf = useHalf;
    }

    public float getBoxConfThreshold() {
        return boxConfThreshold;
    }

    public void setBoxConfThreshold(float boxConfThreshold) {
        this.boxConfThreshold = boxConfThreshold;
    }

    public float getNmsThreshold() {
        return nmsThreshold;
    }

    public void setNmsThreshold(float nmsThreshold) {
        this.nmsThreshold = nmsThreshold;
    }

    public float getRecConfidenceThreshold() {
        return recConfidenceThreshold;
    }

    public void setRecConfidenceThreshold(float recConfidenceThreshold) {
        this.recConfidenceThreshold = recConfidenceThreshold;
    }

    public int getDetLevel() {
        return detLevel;
    }

    public void setDetLevel(int detLevel) {
        this.detLevel = detLevel;
    }
}
