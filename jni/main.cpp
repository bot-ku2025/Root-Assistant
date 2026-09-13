#include <jni.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantStable"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantStable : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Mode kompatibilitas tinggi tanpa membebani CPU (Bebas Lag)
        api->setOption(zygisk::SHOULD_COMPATBLE_CONFIG);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        LOGD("RootAssistant: Clean injection completed.");
    }

private:
    zygisk::Api *api;
};

REGISTER_ZYGISK_MODULE(RootAssistantStable)
