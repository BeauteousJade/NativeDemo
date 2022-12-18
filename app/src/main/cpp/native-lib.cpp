#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>
#include <pthread.h>

#define  LOG_TAG    "pby123_native_lib"
#define  logI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

/**
 * 打印当前class的name.
 * 调用某个类里面的某个步骤：
 * 1. 首先要拿到当前对象的实例，即将jobject转换成为jclass。
 * 2. 拿到对应方法的methodId,这里注意方法签名保证是正确的。
 * 3. 拿到methodId之后，通过调用CallObjectMethod方法来执行，此时需要传入对应类的jobject对象和methodId。
 *
 * 方法签名规则:
 * 1. ()里面的表示参数签名，后面的表示返回类型签名。
 * 2. 如果以一个L开头的描述符，就是类描述符，它后紧跟着类的字符串，然后分号“；”结束。
 * 3. 数组签名以"["开头，例如int[]的签名是：[I 。
 * 4. 如果一个方法返回值为Void,那么签名是V。例如：void test()签名是：()V。
 * 5. 方法参数签名是连续的，例如，void test(int i, int j) 签名是(II)V。
 *
 * 签名对应关系：
 *-------------------------------------------------------------------------
 *| Java类型     | 符号                                                     |
 *--------------------------------------------------------------------------
 *| Boolean     | Z                                                        |
 *--------------------------------------------------------------------------
 *| Byte        | B                                                        |
 *--------------------------------------------------------------------------
 *| Char        | C                                                        |
 *--------------------------------------------------------------------------
 *| Short       | S                                                        |
 *--------------------------------------------------------------------------
 *| Int         | I                                                        |
 *--------------------------------------------------------------------------
 *| Long        | J                                                        |
 *--------------------------------------------------------------------------
 *| Float       | F                                                        |
 *--------------------------------------------------------------------------
 *| Double      | D                                                        |
 *--------------------------------------------------------------------------
 *| Void        | V                                                        |
 *--------------------------------------------------------------------------
 *| objects对象  | 以"L"开头，以";"结尾，中间是用"/" 隔开的包及类名。             |
 *|             | 比如：Ljava/lang/String;如果是嵌套类，则用$来表示嵌套。例如     |
 *|             | "(Ljava/lang/String;Landroid/os/FileUtils$FileStatus;)Z" |
 *-------------------------------------------------------------------------
 *
 * @see:
 * 1. https://blog.csdn.net/mafan121/article/details/50942972
 * 2. https://juejin.cn/post/6844904192780271630
 */
void log(JNIEnv *env, jobject thiz) {
    logI("log");
    // 1. 获取 thiz 的 class，也就是 java 中的 Class 信息
    jclass this_clazz = env->GetObjectClass(thiz);
    // 2. 从class信息里面获取getClass方法的MethId。
    // this_clazz:表示需要获取methodId的对象。
    // getClass:表示方式名称。
    // ()Ljava/lang/Class; :表示方法的签名。
    jmethodID getClass_mid = env->GetMethodID(this_clazz, "getClass", "()Ljava/lang/Class;");

    // 3. 执行getClass方法。获取Class 对象。
    jobject clazz_instance = env->CallObjectMethod(thiz, getClass_mid);

    // 4. 获取Class实例
    jclass clazz = env->GetObjectClass(clazz_instance);

    // 5. 从Class实例中，获取getName的methodId。
    jmethodID getName_mid = env->GetMethodID(clazz, "getName", "()Ljava/lang/String;");
    jstring className = static_cast<jstring>(env->CallObjectMethod(clazz_instance, getName_mid));
    // 6. java  中的String是 unicode 编码， c/C++ 是UTF编码，所以需要转换一下
    logI("%s", env->GetStringUTFChars(className, 0));
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_nativedemo_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    // 打印当前类的类名。
    log(env, thiz);
    std::string hello = "Hello from C++";
    // 返回新的字符串
    return env->NewStringUTF(hello.c_str());
}

/**
 * 更新Java 层传递过来的数组。
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativedemo_MainActivity_updateArray(JNIEnv *env, jobject thiz, jintArray array) {
    // 1. 获取数组长度。
    jint len = env->GetArrayLength(array);
    jboolean isCopy;
    // 2. 获取数组地址。
    // 第二个参数代表java array -> c/c++ array的转换方式：
    //  0:表示Java数组的指针直接传回到本地代码中。
    //  1:表示申请了内存，拷贝了数组。
    // 返回值：数组的地址(首元素地址)
    jint *newArray = env->GetIntArrayElements(array, &isCopy);
    logI("is copy array:%d", isCopy);
    // 3. 遍历数组，且更新。
    for (int i = 0; i < len; ++i) {
        logI("array[%i] = %i", i, *(newArray + i));
        *(newArray + i) = 1;
    }
    // 4. 使用后释放数组。
    // 第一个参数jArray, 第二个参数是GetIntArrayElements返回值。
    // 第二个参数是mode:
    // 0:刷新Java数组，且释放c/c++数组。
    // JNI_COMMIT(1):只刷新Java数组。
    // JNI_ABORT: 只释放c/c++数组。
    env->ReleaseIntArrayElements(array, newArray, 0);
}

/**
 * 打印String
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativedemo_MainActivity_printString(JNIEnv *env, jobject thiz, jstring string) {

    // 1. jstring -> char *
    // java  中的字符创是 unicode 编码， c/C++ 是UTF编码，所以需要转换一下。第二个参数作用同上面
    const char *c_str = env->GetStringUTFChars(string, nullptr);
    // 2. 异常处理
    if (c_str == nullptr) {
        return;
    }

    //3. 打印数组。
    int len = strlen(c_str);
    for (int i = 0; i < len; ++i) {
        logI("c_str:%c", *(c_str + i));
    }

    // 4. 释放
    env->ReleaseStringUTFChars(string, c_str);
}

/**
 * 获取Java的成员变量。c/c++ 操作 java 中的对象使用的是 java 中反射。
 * 注意：
 * 1. 非静态成员变量使用: GetXXXField,比如 GetIntField，对于引用类型，比如 String，使用 GetObjectField。
 * 2. 对于静态成员变量使用: GetStaticXXXField,比如 GetStaticIntField
 *
 *
 * @see
 * log 方法里面介绍了调用Java方法的步骤。
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_nativedemo_MainActivity_updateField(JNIEnv *env, jobject thiz) {

    // 1. 获取class 对象
    jclass clazz = env->GetObjectClass(thiz);

    // 2. 获取fieldId
    jfieldID testStringFieldId = env->GetFieldID(clazz, "mTestString", "Ljava/lang/String;");
    jfieldID testIntFieldId = env->GetStaticFieldID(clazz, "mTestInt", "I");

    // 3. 获取对应Field.
    jstring string = static_cast<jstring>(env->GetObjectField(thiz, testStringFieldId));
    const char *c_str = env->GetStringUTFChars(string, nullptr);
    // 获取static Field.
    jint intField = env->GetStaticIntField(clazz, testIntFieldId);
    logI("MainActivity的mTestString：%s， mTestInt：%d", c_str, intField);

    // 4. 设置新值
    env->SetStaticIntField(clazz, testIntFieldId, 123);
    jstring newValue = env->NewStringUTF("新的字符串");
    env->SetObjectField(thiz, testStringFieldId, newValue);

    // 5. 释放资源
    env->ReleaseStringUTFChars(string, c_str);
    env->DeleteLocalRef(newValue);
    env->DeleteLocalRef(clazz);

}

/**
 * 创建一个Java对象。
 */
extern "C" JNIEXPORT jobject JNICALL
Java_com_example_nativedemo_MainActivity_newObject(JNIEnv *env, jobject thiz) {

    // 1. 获取class
    jclass personClazz = env->FindClass("com/example/nativedemo/Person");
    // 2. 获取构造方法的methodId。构造方法的方法名固定为<init>, 返回值为void。
    jmethodID constructId = env->GetMethodID(personClazz, "<init>", "(ILjava/lang/String;)V");
    if (constructId == nullptr) {
        return nullptr;
    }
    // 3. 创建对象。
    jstring name = env->NewStringUTF("pby");
    jobject object = env->NewObject(personClazz, constructId, 20, name);

    // 4. 释放资源。
    env->DeleteLocalRef(name);
    env->DeleteLocalRef(personClazz);
    // 这里不能删除，否则会为空。
//    env->DeleteLocalRef(object);
    return object;
}


/**
 * 创建引用
 */
void createRef(JNIEnv *env, jobject thiz) {
    // 1. 从一个局部引用创建全局引用。

    static jstring globalStr = nullptr;
    jstring str = env->NewStringUTF("pby123");
    globalStr = static_cast<jstring>(env->NewGlobalRef(str));

    env->DeleteLocalRef(str);
    env->DeleteGlobalRef(globalStr);

    // 2. 创建一个全局弱引用
    static jclass globalClazz = nullptr;
    //对于弱引用 如果引用的对象被回收返回 true，否则为false
    //对于局部和全局引用则判断是否引用java的null对象
    jboolean isEqual = env->IsSameObject(globalClazz, nullptr);
    if (globalClazz == nullptr || isEqual) {
        jclass clazz = env->GetObjectClass(thiz);
        globalClazz = static_cast<jclass>(env->NewWeakGlobalRef(clazz));
        env->DeleteLocalRef(clazz);
    }
    env->DeleteWeakGlobalRef(globalClazz);
}


JavaVM *_vm;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    _vm = vm;
    return JNI_VERSION_1_6;
}

void *threadTask(void *args) {
    JNIEnv *env;
    // 通过AttachCurrentThread 初始化 JNIEnv。
    // JNIEnv是线程独占的。
    jint result = _vm->AttachCurrentThread(&env, 0);
    if (result != JNI_OK) {
        return 0;
    }
    logI("启动了一个线程");

    _vm->DetachCurrentThread();
    // 需要 return ，否则会崩溃
    return 0;
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_nativedemo_MainActivity_startThread(JNIEnv *env, jobject thiz) {
    pthread_t pid;
    pthread_create(&pid, 0, threadTask, 0);
}