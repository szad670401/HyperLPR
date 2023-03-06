package com.hyperai.hyperlpr3.bean;

import com.hyperai.hyperlpr3.settings.TypeDefine;

public class HyperLPRParameter {

    private String modelPath;

    private int threads = 1;

    private boolean useHalf = true;

    private float boxConfThreshold = 0.25f;

    private float nmsThreshold = 0.6f;

    private float recConfidenceThreshold = 0.85f;

    private int detLevel = TypeDefine.DETECT_LEVEL_LOW;

    private int maxNum = 3;

    public HyperLPRParameter() {
    }

    public int getMaxNum() {
        return maxNum;
    }

    public HyperLPRParameter setMaxNum(int maxNum) {
        this.maxNum = maxNum;
        return this;
    }

    public String getModelPath() {
        return modelPath;
    }

    public HyperLPRParameter setModelPath(String modelPath) {
        this.modelPath = modelPath;
        return this;
    }

    public int getThreads() {
        return threads;
    }

    public HyperLPRParameter setThreads(int threads) {
        this.threads = threads;
        return this;
    }

    public boolean isUseHalf() {
        return useHalf;
    }

    public HyperLPRParameter setUseHalf(boolean useHalf) {
        this.useHalf = useHalf;
        return this;
    }

    public float getBoxConfThreshold() {
        return boxConfThreshold;
    }

    public HyperLPRParameter setBoxConfThreshold(float boxConfThreshold) {
        this.boxConfThreshold = boxConfThreshold;
        return this;
    }

    public float getNmsThreshold() {
        return nmsThreshold;
    }

    public HyperLPRParameter setNmsThreshold(float nmsThreshold) {
        this.nmsThreshold = nmsThreshold;
        return this;
    }

    public float getRecConfidenceThreshold() {
        return recConfidenceThreshold;
    }

    public HyperLPRParameter setRecConfidenceThreshold(float recConfidenceThreshold) {
        this.recConfidenceThreshold = recConfidenceThreshold;
        return this;
    }

    public int getDetLevel() {
        return detLevel;
    }

    public HyperLPRParameter setDetLevel(int detLevel) {
        this.detLevel = detLevel;
        return this;
    }


}
