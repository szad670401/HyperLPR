#!/bin/bash

set -ex
RELEASE_HOME=$(cd $(dirname $0)/..; pwd)

BUILD_3RDPARTY_PATH="${RELEASE_HOME}/3rdparty_hyper_inspire_op/"
OPENCV_DIR=${BUILD_3RDPARTY_PATH}/opencv-4.5.1/opencv-4.5.1-android-sdk/sdk/native/jni

BUILD_DIR=${RELEASE_HOME}/build/release_android

[[ -d ${BUILD_DIR} ]] && rm -r ${BUILD_DIR}

build() {
    arch=$1
    NDK_API_LEVEL=$2
    mkdir -p ${BUILD_DIR}/${arch}
    pushd ${BUILD_DIR}/${arch}
    cmake ${RELEASE_HOME} \
        -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake \
        -DANDROID_TOOLCHAIN=clang \
        -DANDROID_ABI=${arch} \
        -DANDROID_NATIVE_API_LEVEL=${NDK_API_LEVEL} \
        -DANDROID_STL=c++_static \
        -DBUILD_CUDA=OFF \
        -DBUILD_SAMPLES=OFF \
        -DBUILD_TEST=OFF \
        -DOpenCV_DIR=${OPENCV_DIR} \
        -DMNN_LIBS=${BUILD_3RDPARTY_PATH}/MNN-2.2.0/android-static/${arch} \
        -DMNN_INCLUDE_DIRS=${BUILD_3RDPARTY_PATH}/MNN-2.2.0/android-static/include
#        -DNCNN_DIR=${RELEASE_HOME}/3rdparty/ncnn/android/${arch} \
    make -j$(nproc) hyperlpr3
    ls ${BUILD_DIR}/${arch}| grep -v so| xargs rm -r
    #make -j$(nproc) track_tool
    popd
}


build arm64-v8a 21
build armeabi-v7a 21

date -R > ${BUILD_DIR}/release_note.txt
cd ${BUILD_DIR}
find . -type f |xargs md5sum >>release_note.txt
cd -

