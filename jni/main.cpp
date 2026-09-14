#include <jni.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "RootAssistant", __VA_ARGS__)

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        LOGD("Root Assistant Zygisk loaded successfully!");
    }
    
    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Logika saat aplikasi di-spesialisasi
    }
};

REGISTER_ZYGISK_MODULE(MyModule)
