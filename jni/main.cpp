#include <jni.h>
#include <android/log.h>
#include <sys/system_properties.h>
#include <cstdlib>
#include <unistd.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onModuleLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("Root-Assistant v1.7-Phase7 Zygisk module loaded successfully.");
    }

    void preSpecializeApp(zygisk::AppSpecializeArgs *args) override {
        LOGD("Root-Assistant: preSpecializeApp hook active.");
    }

    void postSpecializeApp(const zygisk::AppSpecializeArgs *args) override {
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
