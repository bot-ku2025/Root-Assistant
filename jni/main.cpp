#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant initialized successfully in Zygisk environment.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Logika sebelum aplikasi diinisialisasi (Sandbox / Pre-specialize)
        LOGD("Pre-app specialization hook triggered.");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Logika setelah aplikasi berjalan penuh
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        // Logika untuk proses sistem (system_server)
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

// Daftarkan modul ke entry point Zygisk
REGISTER_ZYGISK_MODULE(RootAssistantModule)
