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
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    LOGD("Target Application Detected -> Package: %s", name);
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantPhase3)
