#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/stat.h>
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
        if (args && args->uid >= 10000) {
            hide_root = true;
        } else {
            hide_root = false;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (hide_root) {
            // Daftar jalur inti yang aman untuk diperiksa dan di-unmount
            std::vector<std::string> target_paths = {
                "/data/adb", 
                "/system/bin/su", 
                "/system/xbin/su", 
                "/sbin/su",
                "/data/local/tmp", 
                "/data/magisk", 
                "/sbin/magisk", 
                "/data/misc/apatch"
            };

            struct stat st;
            for (const auto& path : target_paths) {
                // Hanya unmount jika jalur tersebut benar-benar ada fisik/mount-nya
                if (stat(path.c_str(), &st) == 0) {
                    umount2(path.c_str(), MNT_DETACH);
                }
            }

            unsetenv("_REJECT_MAGISK_HIDE");
            unsetenv("MAGISK_INJECT_LOG_LEVEL");
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
    bool hide_root = false;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
