#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantCore : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant Core initialized successfully in Zygisk context.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        LOGD("Pre-app specialization hook active.");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Ruang eksekusi setelah aplikasi berjalan
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization intercepted.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantCore)
