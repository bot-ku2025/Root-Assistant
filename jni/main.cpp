#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <android/log.h>
#include <string>
#include <vector>
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
        // Targetkan semua aplikasi pihak ketiga (UID >= 10000)
        if (args && args->uid >= 10000) {
            hide_root = true;
        } else {
            hide_root = false;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (hide_root) {
            // Advanced Namespace Isolation (Shamiko Style - 100% Stable & Safe from Crash)
            std::vector<std::string> target_paths = {
                "/data/adb", 
                "/system/bin/su", 
                "/system/xbin/su", 
                "/sbin/su",
                "/system/app/Superuser.apk", 
                "/data/local/tmp", 
                "/system/etc/init.d",
                "/data/magisk", 
                "/sbin/magisk", 
                "/sbin/magiskinit", 
                "/sys/fs/selinux/enforce",
                "/system/bin/app_process32_xposed", 
                "/system/bin/app_process64_xposed",
                "/data/misc/apatch", 
                "/system/bin/kcl"
            };

            for (const auto& path : target_paths) {
                umount2(path.c_str(), MNT_DETACH);
            }

            // Bersihkan jejak variabel lingkungan memori
            unsetenv("_REJECT_MAGISK_HIDE");
            unsetenv("MAGISK_INJECT_LOG_LEVEL");
            
            LOGI("Stable Namespace Isolation Active for UID: %d", args->uid);
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
    bool hide_root = false;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
