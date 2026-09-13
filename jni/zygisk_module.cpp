#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantBypass"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class RootAssistantUltimate : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;

        const char *process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (!process_name) return;

        // Targetkan BCA Mobile dan aplikasi finansial lainnya
        if (strstr(process_name, "com.bca") || 
            strstr(process_name, "ovo.id") || 
            strstr(process_name, "alfamart")) {
            
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
            LOGI("Target hooked & unmounted for process: %s", process_name);
        }

        env->ReleaseStringUTFChars(args->nice_name, process_name);
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;
        const char *process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (!process_name) return;

        // Jika proses adalah BCA Mobile, lakukan spoofing variabel lingkungan dan properti runtime
        if (strstr(process_name, "com.bca")) {
            setenv("ACCESSIBILITY_ENABLED", "0", 1);
            LOGI("Neutralizing Accessibility checks for BCA");
        }

        env->ReleaseStringUTFChars(args->nice_name, process_name);
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantUltimate)
