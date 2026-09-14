#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantPhase3 : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant Phase 3 loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Mengekstrak nama paket aplikasi secara aman dari argumen Zygisk jika tersedia
        // Menggunakan pointer aman untuk mencegah crash pada berbagai versi Android
        if (args && args->nice_name) {
            const char *nice_name = env->GetStringUTFChars(*args->nice_name, nullptr);
            if (nice_name) {
                LOGD("Target Application Detected -> Package: %s", nice_name);
                env->ReleaseStringUTFChars(*args->nice_name, nice_name);
            }
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Ruang eksekusi pasca-spesialisasi aplikasi
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantPhase3)
