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
            // 1. Perintahkan KernelSU/Magisk daemon untuk melakukan Unmount jalur root secara aman (bebas crash)
            api->setOption(zygisk::Option::FORCE_DENYLIST_UNMOUNT);
            
            // 2. Cabut/Hapus modul Root-Assistant dari memori aplikasi ini agar tidak terdeteksi detektor memori
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
            
            LOGI("Native Zygisk Delegation active for UID: %d", args->uid);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // KOSONGKAN! Jangan gunakan syscall unshare/umount2 di sini karena memicu Seccomp FATAL EXCEPTION.
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
