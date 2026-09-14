#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    LOGD("Target App -> %s", name);
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialized.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

// Pendaftaran entry point Zygisk yang kompatibel dengan standar header modern
static void register_module(zygisk::Api *api) {
    zygisk::ModuleBase *module = new RootAssistantModule();
    api->registerModule(module);
}

// Ekspor fungsi entry point Zygisk secara eksplisit
extern "C" {
__attribute__((visibility("default"))) __attribute__((used))
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) {
    zygisk::Api::init(api, env);
    register_module(api);
}
}
