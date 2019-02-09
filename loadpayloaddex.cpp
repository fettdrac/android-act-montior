//
// 这个小工具自己写的，就是libinject2注入目标进程后加载个dex执行下里面的一个方法
// java中毒已深，疯狂反射JAVA方法
// Created by FKD on 2019/2/2.
//
#include <jni.h>
#include <dlfcn.h>
#include <android/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define AKLog(...) __android_log_print(ANDROID_LOG_VERBOSE, __FUNCTION__, __VA_ARGS__)

JNIEnv*(*getAndroidRuntimeEnv)();
JNIEnv* getJNIEnvByDLSYM() {
    void*runtime = dlopen("/system/lib/libandroid_runtime.so", RTLD_NOW);
    getAndroidRuntimeEnv= (JNIEnv*(*)())dlsym(runtime, "_ZN7android14AndroidRuntime9getJNIEnvEv");
    return getAndroidRuntimeEnv();
}

jobject getApplicationJNI(){
    JNIEnv *env=getJNIEnvByDLSYM();
    //获取Application实例
    jclass atClass = env->FindClass("android/app/ActivityThread");
    jmethodID caMethod = env->GetStaticMethodID(atClass,"currentApplication","()Landroid/app/Application;");
    jobject thisApp = env->CallStaticObjectMethod(atClass,caMethod);
    return thisApp;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_pvdnc_psvision_injectdex_Native_getApplicationJNI(JNIEnv *env_bak,jobject){
    return getApplicationJNI();
}

jstring getDirJNI(jstring dirName) {
    JNIEnv *env = getJNIEnvByDLSYM();
    //获取dirFile
    jclass cClass = env->FindClass(
            "android/content/Context");//对于有SuperClass(extends标志的类）调用SuperClass类的方法时直接去SuperClass里面找
    jmethodID gMethod = env->GetMethodID(cClass, "getDir", "(Ljava/lang/String;I)Ljava/io/File;");
    AKLog("context.getDir method address:%p\n", gMethod);
    jobject dirFile = env->CallObjectMethod(getApplicationJNI(), gMethod, dirName, 0);
    AKLog("got dirFile address:%p\n", dirFile);
    //获取dirFile的绝对路径
    jclass fClass = env->FindClass("java/io/File");
    jmethodID gapMethod = env->GetMethodID(fClass, "getAbsolutePath", "()Ljava/lang/String;");
    jstring absPath = (jstring) (env->CallObjectMethod(dirFile, gapMethod));
    AKLog("dir absPath:%s\n",env->GetStringUTFChars(absPath,NULL));
    return absPath;
}

extern "C" JNIEXPORT jstring JNICALL Java_com_pvdnc_psvision_injectdex_Native_getDirJNI(JNIEnv *env_bak,jobject,jstring dirName){
    return getDirJNI(dirName);
}

jobject getSystemClassLoaderJNI(){
    JNIEnv *env=getJNIEnvByDLSYM();
    jclass cClass=env->FindClass("java/lang/ClassLoader");
    jmethodID method=env->GetStaticMethodID(cClass,"getSystemClassLoader","()Ljava/lang/ClassLoader;");
    if(!method){
        AKLog("cannot find method:getSystemClassLoader");
        return NULL;
    }
    jobject classLoader=env->CallStaticObjectMethod(cClass,method);
    AKLog("got systemClassLoader address:%p\n",classLoader);
    return classLoader;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_pvdnc_psvision_injectdex_Native_getSystemClassLoaderJNI(JNIEnv *env_bak,jobject){
    return getSystemClassLoaderJNI();
}

jobject getAppClassLoaderJNI(){
    JNIEnv *env=getJNIEnvByDLSYM();
    //获取AppClassLoader
    jclass cClass=env->FindClass("android/content/Context");//对于有SuperClass(extends标志的类）调用SuperClass类的方法时直接去SuperClass里面找
    jmethodID gclMethod=env->GetMethodID(cClass,"getClassLoader","()Ljava/lang/ClassLoader;");
    AKLog("context.getClassLoader method address:%p\n",gclMethod);
    jobject classLoader=env->CallObjectMethod(getApplicationJNI(),gclMethod);
    AKLog("appClassLoader address:%p\n",classLoader);
    return classLoader;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_pvdnc_psvision_injectdex_Native_getAppClassLoaderJNI(JNIEnv *env_bak,jobject){
    return getAppClassLoaderJNI();
}

#define JSTRING_SIG "Ljava/lang/String;"
#define JCLASSLOADER_SIG "Ljava/lang/ClassLoader;"
jobject getDexClassLoaderJNI(jstring dexPath,jstring dexOptPath,jstring libPath,jobject classLoader){
    JNIEnv *env=getJNIEnvByDLSYM();
    const char* initSig="(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V";
    AKLog("dexClassLoader <init> sig:%s\n",initSig);
    jclass dclClass=env->FindClass("dalvik/system/DexClassLoader");
    jmethodID constructID= env->GetMethodID(dclClass,"<init>",initSig);
    jobject newDCL=env->NewObject(dclClass,constructID,
                                  dexPath,dexOptPath,libPath,classLoader);
    AKLog("got dexClassLoader instance address:%p\n",newDCL);
    return newDCL;
}

jobject getDexClassLoaderJNI(const char* dexPath,const char* dexOptPath,const char* libPath,jobject classLoader){
    JNIEnv *env=getJNIEnvByDLSYM();
    const char* initSig="(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/ClassLoader;)V";
    AKLog("dexClassLoader <init> sig:%s\n",initSig);
    jclass dclClass=env->FindClass("dalvik/system/DexClassLoader");
    jmethodID constructID= env->GetMethodID(dclClass,"<init>",initSig);
    jobject newDCL=env->NewObject(dclClass,constructID,
                                  env->NewStringUTF(dexPath),
                                  env->NewStringUTF(dexOptPath),
                                  env->NewStringUTF(libPath),
                                  classLoader);//这里classLoader最好用appClassLoader，用systemClassLoader有些反射没法用
    AKLog("got dexClassLoader instance address:%p\n",newDCL);
    return newDCL;
}

extern "C" JNIEXPORT jobject JNICALL Java_com_pvdnc_psvision_injectdex_Native_getDexClassLoaderJNI(JNIEnv *env_bak,jobject,
                                                                                                    jstring dexPath,jstring dexOptPath,jstring libPath,jboolean useAppClassLoader){
    if(useAppClassLoader){
        AKLog("choose to use appClassLoader\n");
        return getDexClassLoaderJNI(dexPath,dexOptPath,libPath,getAppClassLoaderJNI());
    }else{
        AKLog("choose to use systemClassLoader\n");
        return getDexClassLoaderJNI(dexPath,dexOptPath,libPath,getSystemClassLoaderJNI());
    }
}

void runPayloadStartJNI(jobject dexClassLoader,jstring className/*这个参数里的类名/包名用.分割*/){
    JNIEnv *env=getJNIEnvByDLSYM();
    //获取payloadClass
    const char* sig="(Ljava/lang/String;)Ljava/lang/Class;";
    jclass cClass=env->FindClass("dalvik/system/DexClassLoader");
    jmethodID lcMethod=env->GetMethodID(cClass,"loadClass",sig);
    jclass payloadClass=(jclass)env->CallObjectMethod(dexClassLoader,lcMethod,className);
    AKLog("payloadClass address:%p\n",payloadClass);
    //调用Payload.start()静态方法
    jmethodID startMethod=env->GetStaticMethodID(payloadClass,"start","()V");
    AKLog("startMethod address:%p\n",startMethod);
    env->CallStaticVoidMethod(payloadClass,startMethod);
    AKLog("callStaticVoidMethod finished");
}

extern "C" JNIEXPORT void JNICALL Java_com_pvdnc_psvision_injectdex_Native_runPayloadStartJNI(JNIEnv *env_bak,jobject,
                                                                                               jobject dexClassLoader,jstring className){
    runPayloadStartJNI(dexClassLoader,className);
}
static bool isLoaded;
void loadDexEntry(const char* dexPath) {//汇总方法，一步到位
    if(isLoaded) {
        AKLog("dex:%s might be loaded\n",dexPath);
        return;
    }
    isLoaded=true;
    JNIEnv *env = getJNIEnvByDLSYM();
    const char *dexOptPath = env->GetStringUTFChars(getDirJNI(env->NewStringUTF("optDex")), NULL);
    AKLog("dexOptPath is:%s\n", dexOptPath);
    jobject dexClassLoader = getDexClassLoaderJNI(dexPath, dexOptPath, NULL,
                                                  getAppClassLoaderJNI());
    const char* payloadClassName="com.injectdex.Payload";//发版的时候InjectDex必须要有这个类
    runPayloadStartJNI(dexClassLoader, env->NewStringUTF(payloadClassName));
    AKLog("loadDexEntry finished\n");
}

extern "C" JNIEXPORT void JNICALL Java_com_pvdnc_psvision_injectdex_Native_loadDexEntry(JNIEnv *env_bak,jobject,jstring dexPath) {
    JNIEnv *env = getJNIEnvByDLSYM();
    loadDexEntry(env->GetStringUTFChars(dexPath, NULL));
}

class static_initializer {//在Application.OnCreate之后调用，或在so被dlopen之后调用
public:
    static_initializer() {
        AKLog("static_initializer 执行\n");
        loadDexEntry("/system/test.dex");//算是hardcode了，不过能注入的都可以写/system
    }
};
static static_initializer s;

extern "C"/*作为任何外部调用的函数，必须要有extern "C"！*/ JNIEXPORT int hook_entry(char * a) {
    AKLog("Hook success, pid = %d\n", getpid());
    AKLog("we are going to load dex:%s\n", a);
    loadDexEntry(a);
    return 0;
}
