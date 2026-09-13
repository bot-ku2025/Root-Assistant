#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class RootAssistantZygisk : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;

        const char *process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (!process_name) return;

        if (strstr(process_name, "com.bca") || 
            strstr(process_name, "ovo.id") || 
            strstr(process_name, "alfamart")) {
            
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
        }

        env->ReleaseStringUTFChars(args->nice_name, process_name);
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        // Netralkan status Aksesibilitas & Developer Options di lingkungan aplikasi target
        setenv("ACCESSIBILITY_ENABLED", "0", 1);
        setenv("DEVELOPMENT_SETTINGS_ENABLED", "0", 1);
        setenv("ADB_ENABLED", "0", 1);
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantZygisk)
