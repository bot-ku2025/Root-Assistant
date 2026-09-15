#include <sys/types.h>
#include <sys/stat.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "Root-Assistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->uid >= 10000) {
            // Senjata 1: Perintahkan Zygisk unmount secara aman tanpa Seccomp Crash
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
            
            // Hapus modul dari memori (Stealth)
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
            
            LOGI("Native Zygisk Delegation active for UID: %d", args->uid);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Biarkan kosong agar tidak memicu Seccomp FATAL EXCEPTION
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
