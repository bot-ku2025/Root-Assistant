#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantPhase2 : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant Phase 2 loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Mengambil data nama paket aplikasi (process name) secara dinamis dari argumen Zygisk
        // Di sini kita bisa menyaring aplikasi apa saja yang sedang dimuat
        LOGD("Pre-app specialization triggered for process execution.");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Ruang aman pasca-inisialisasi aplikasi
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantPhase2)
