#include <sys/types.h>
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
        // HAPUS SEMUA Opsi DLCLOSE / Unmount yang memicu crash. 
        // Biarkan Zygisk berjalan bersih tanpa mengganggu runtime aplikasi.
        if (args && args->uid >= 10000) {
            LOGI("Safe Pass-Through active for UID: %d", args->uid);
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {}

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
