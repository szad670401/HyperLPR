//
// Created by tunm on 2023/1/26.
//

#ifndef ZEPHYRLPR_JNI_UTILS_H
#define ZEPHYRLPR_JNI_UTILS_H

namespace hyper {

#define HYPERLPR_API(sig) Java_com_hyperai_hyperlpr3_##sig

inline jstring stringTojstring(JNIEnv *env, const char *pat) {

    jclass strClass = (env)->FindClass("java/lang/String");
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = (env)->NewByteArray(strlen(pat));
    (env)->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte *) pat);
    jstring encoding = (env)->NewStringUTF("GBK");
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);

}


inline jstring JNIString_getString(JNIEnv *env, const char *c_str) {

        //C 返回 java 字符串
        jclass str_cls = (env)->FindClass("java/lang/String");
        jmethodID jmid = (env)->GetMethodID(str_cls, "<init>", "([BLjava/lang/String;)V");

        //jstring -> jbyteArray
        jbyteArray bytes = (env)->NewByteArray(strlen(c_str));
        // 将Char * 赋值到 bytes
        (env)->SetByteArrayRegion(bytes, 0, strlen(c_str), (jbyte *) c_str);
        jstring charsetName = (env)->NewStringUTF("GB2312");

        return (jstring)(env)->NewObject(str_cls, jmid, bytes, charsetName);

}


inline std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid =
            env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

} // namespace

#endif //ZEPHYRLPR_JNI_UTILS_H
