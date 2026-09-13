#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantZygisk"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        api->setOption(zygisk::SHOULD_COMPATBLE_CONFIG);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        LOGD("RootAssistant Zygisk active in app process. Hardening runtime accessibility queries.");
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
