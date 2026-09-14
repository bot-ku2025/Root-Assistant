#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <android/log.h>
#include <string>
#include <vector>
#include <cerrno>
#include "zygisk.hpp"

#define LOG_TAG "Root-Assistant-Ultimate"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// ==========================================
// SYSCALL HOOKING (Zygisk Assistant Style)
// ==========================================
static int (*orig_faccessat)(int dirfd, const char *pathname, int mode, int flags);
static int my_faccessat(int dirfd, const char *pathname, int mode, int flags) {
    if (pathname != nullptr) {
        std::string path(pathname);
        if (path.find("su") != std::string::npos || path.find("magisk") != std::string::npos || path.find("kernelsu") != std::string::npos) {
            errno = ENOENT; // Bohong: File tidak ditemukan
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

// ==========================================
// MAIN ZYGISK ENGINE
// ==========================================
class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Universal Smart Filtering: Aktifkan hide untuk semua User Apps (UID > 10000)
        if (args->uid >= 10000) {
            hide_root = true;
            
            // Daftarkan PLT Hooks untuk menipu pembacaan memori
            api->pltHookRegister(".*", "faccessat", (void*)my_faccessat, (void**)&orig_faccessat);
            api->pltHookRegister(".*", "openat", (void*)my_openat, (void**)&orig_openat);
        } else {
            hide_root = false;
        }
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (hide_root) {
            // Terapkan semua Hook yang sudah didaftarkan
            if (api->pltHookCommit()) {
                LOGI("Syscall Hooking Berhasil Diterapkan.");
            }

            // Advanced Namespace Isolation (Shamiko Style)
            std::vector<std::string> target_paths = {
                "/data/adb", "/system/bin/su", "/system/xbin/su", "/sbin/su",
                "/system/app/Superuser.apk", "/data/local/tmp", "/system/etc/init.d",
                "/data/magisk", "/sbin/magisk", "/sbin/magiskinit", "/sys/fs/selinux/enforce",
                "/system/bin/app_process32_xposed", "/system/bin/app_process64_xposed",
                "/data/misc/apatch", "/system/bin/kcl"
            };

            for (const auto& path : target_paths) {
                // Cabut paksa jalur root dari memori proses ini
                umount2(path.c_str(), MNT_DETACH);
            }

            // Environment Scrubbing: Bersihkan variabel memori yang mencurigakan
            unsetenv("_REJECT_MAGISK_HIDE");
            unsetenv("MAGISK_INJECT_LOG_LEVEL");
            
            LOGI("Mode Stealth Maksimal Aktif untuk UID: %d", args->uid);
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
    bool hide_root = false;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
