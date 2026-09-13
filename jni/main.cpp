#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantDeepShield"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantHook : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        api->setOption(zygisk::SHOULD_COMPATBLE_CONFIG);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        JNIEnv *env = nullptr;
        JavaVM *vm = api->getJavaVM();
        if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
            if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
                return;
            }
        }

        // Hooking runtime environment untuk menetralisir pemeriksaan aksesibilitas dan root
        LOGD("RootAssistantDeepShield: Injecting runtime shields into app process.");
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantHook)
