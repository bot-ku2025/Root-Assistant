#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class RootAssistantMajorV3 : public zygisk::ModuleBase {
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
            strstr(process_name, "alfamart") ||
            strstr(process_name, "topjohnwu") ||
            strstr(process_name, "security") ||
            strstr(process_name, "momo") ||
            strstr(process_name, "root")) {
            
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
        }
        env->ReleaseStringUTFChars(args->nice_name, process_name);
    }

    void postAppSpecialize(const AppSpecializeArgs *args) override {
        setenv("ACCESSIBILITY_ENABLED", "0", 1);
        setenv("RO_DEBUGGABLE", "0", 1);
        setenv("RO_SECURE", "1", 1);
    }
private:
    Api *api;
    JNIEnv *env;
};
REGISTER_ZYGISK_MODULE(RootAssistantMajorV3)
