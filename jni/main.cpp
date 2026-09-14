#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <android/log.h>
#include <string>
#include <vector>
#include "zygisk.hpp"

#define LOG_TAG "Root-Assistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

static int (*orig_faccessat)(int dirfd, const char *pathname, int mode, int flags);
static int my_faccessat(int dirfd, const char *pathname, int mode, int flags) {
    if (pathname != nullptr) {
        std::string path(pathname);
        if (path.find("su") != std::string::npos || path.find("magisk") != std::string::npos || path.find("kernelsu") != std::string::npos) {
            errno = ENOENT; // Manipulasi output menjadi: File Not Found
            return -1;
        }
    }
    return orig_faccessat(dirfd, pathname, mode, flags);
}

static int (*orig_openat)(int dirfd, const char *pathname, int flags, mode_t mode);
static int my_openat(int dirfd, const char *pathname, int flags, mode_t mode) {
    if (pathname != nullptr) {
        std::string path(pathname);
        if (path.find("su") != std::string::npos || path.find("magisk") != std::string::npos || path.find("kernelsu") != std::string::npos) {
            errno = ENOENT;
            return -1;
        }
    }
    return orig_openat(dirfd, pathname, flags, mode);
}

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Universal Smart Filtering (Target UID > 10000)
        if (args && args->uid >= 10000) {
            hide_root = true;
            
            // Perbaikan Zygisk API: Pakai 0, 0 untuk menembus semua loaded libraries (universal hook)
            api->pltHookRegister(0, 0, "faccessat", (void*)my_faccessat, (void**)&orig_faccessat);
            api->pltHookRegister(0, 0, "openat", (void*)my_openat, (void**)&orig_openat);
        } else {
            hide_root = false;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (hide_root) {
            // Eksekusi Hook System Call
            api->pltHookCommit();

            // Eksekusi Isolasi Namespace (Shamiko Style)
            std::vector<std::string> target_paths = {
                "/data/adb", "/system/bin/su", "/system/xbin/su", "/sbin/su",
                "/system/app/Superuser.apk", "/data/local/tmp", "/system/etc/init.d",
                "/data/magisk", "/sbin/magisk", "/sbin/magiskinit", "/sys/fs/selinux/enforce",
                "/system/bin/app_process32_xposed", "/system/bin/app_process64_xposed",
                "/data/misc/apatch", "/system/bin/kcl"
            };

            for (const auto& path : target_paths) {
                umount2(path.c_str(), MNT_DETACH);
            }

            // Bersihkan Jejak Memory Environtment
            unsetenv("_REJECT_MAGISK_HIDE");
            unsetenv("MAGISK_INJECT_LOG_LEVEL");
            
            LOGI("Ultimate Stealth Active! Syscall hooked & paths unmounted.");
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
    bool hide_root = false;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
