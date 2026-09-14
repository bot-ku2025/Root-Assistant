#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

class RootAssistantAdvanced : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant Advanced Module loaded successfully into memory.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Blok ini berjalan sebelum proses aplikasi (APK) diinisialisasi
        // Sangat aman untuk menyisipkan manipulasi lingkungan runtime di sini
        LOGD("Pre-app specialization hook executed securely.");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Blok ini berjalan tepat setelah aplikasi aktif di ruang memorinya sendiri
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        // Blok ini berjalan khusus pada lingkup system_server Android
        LOGI("System server specialization intercepted and secured.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

// Mendaftarkan modul ke entry point utama Zygisk
REGISTER_ZYGISK_MODULE(RootAssistantAdvanced)
