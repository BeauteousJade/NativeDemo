
#include <jni.h>
#include <string>
#include <android/log.h>


#define  LOG_TAG    "pby123_native_lib"
#define  logI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

int dynamicNativeFun(JNIEnv *env, jobject thiz) {
    logI("调用了dynamicNativeFun");
    return -1;
}

const static JNINativeMethod methods[] = {
        {"dynamicNativeFun", "()I", (int *) dynamicNativeFun}
};

const char *className = "com/example/nativedemo/MainActivity";

/**
 * JNI 方法动态注册。
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    // 1. 获取 JNIEnv，这个地方要注意第一个参数是个二级指针
    int result = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    logI("result:%d, env:%d", result, (env == nullptr));
    // 2. 是否获取成功
    if (result != JNI_OK) {
        logI("获取 env 失败");
        return JNI_VERSION_1_6;
    }
//    // 3. 注册方法
    jclass classMainActivity = env->FindClass(className);

    result = env->RegisterNatives(classMainActivity, methods,
                                  sizeof(methods) / sizeof(JNINativeMethod));
    if (result != JNI_OK) {
        logI("注册方法失败");
        return JNI_VERSION_1_2;
    }
    return JNI_VERSION_1_6;
}

