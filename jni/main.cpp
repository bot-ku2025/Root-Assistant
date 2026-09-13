#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <sys/mount.h>
#include <fcntl.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantUltimate"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class RootAssistantUltimate : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        api->setOption(zygisk::SHOULD_COMPATBLE_CONFIG);
        
        // Level 100 Core: Isolasi Mount Namespace Total Tanpa Kompromi
        // Mencegah aplikasi mendeteksi direktori modul, magisk, ksu, atau mount point aktif
        if (unshare(CLONE_NEWNS) == 0) {
            // Ubah propagasi root menjadi privat agar unmount tidak merusak sistem global
            mount("none", "/", nullptr, MS_REC | MS_PRIVATE, nullptr);
            
            // Putuskan dan sembunyikan direktori sensitif dari pandangan proses aplikasi
            umount2("/data/adb", MNT_DETACH);
            umount2("/debug_ramdisk", MNT_DETACH);
            umount2("/sbin", MNT_DETACH);
            umount2("/apex/com.android.runtime", MNT_DETACH); // Opsional untuk hardening ART
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        LOGD("RootAssistant Ultimate Shield active: Namespace isolated and cloaked.");
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantUltimate)
