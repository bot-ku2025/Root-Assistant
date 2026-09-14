#include <jni.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <fcntl.h>
#include "zygisk.hpp"

// ==========================================
// 1. PROPERTY SANITIZATION (Sembunyikan Sentinel & Root Props)
// ==========================================
static int (*orig___system_property_get)(const char *name, char *value);
static int hooked___system_property_get(const char *name, char *value) {
    if (name != nullptr) {
        if (strncmp(name, "persist.sys.sentinel.", 21) == 0 ||
            strstr(name, "magisk") != nullptr ||
            strstr(name, "kernelsu") != nullptr ||
            strstr(name, "ksu") != nullptr ||
            strstr(name, "modversion") != nullptr) {
            if (value != nullptr) {
                value[0] = '\0';
            }
            return 0;
        }
    }
    return orig___system_property_get(name, value);
}

// ==========================================
// 2. VFS & SYSFS CLOAKING (Blokir Akses KCAL, Simple_LMK, KSUN, & Folder ADB)
// ==========================================
static bool is_sensitive_path(const char *pathname) {
    if (pathname == nullptr) return false;
    return (strstr(pathname, "ModuleSentinel") != nullptr ||
            strstr(pathname, "lspd") != nullptr ||
            strstr(pathname, "tricky_store") != nullptr ||
            strstr(pathname, "zygisksu") != nullptr ||
            strstr(pathname, "kcal") != nullptr ||
            strstr(pathname, "simple_lmk") != nullptr ||
            strstr(pathname, "ksun") != nullptr ||
            strstr(pathname, "sentinel") != nullptr);
}

static struct dirent* (*orig_readdir)(DIR *dirp);
static struct dirent* hooked_readdir(DIR *dirp) {
    struct dirent* entry = orig_readdir(dirp);
    while (entry != nullptr) {
        if (is_sensitive_path(entry->d_name)) {
            entry = orig_readdir(dirp);
            continue;
        }
        break;
    }
    return entry;
}

static int (*orig_access)(const char *pathname, int mode);
static int hooked_access(const char *pathname, int mode) {
    if (is_sensitive_path(pathname)) {
        errno = ENOENT;
        return -1;
    }
    return orig_access(pathname, mode);
}

static int (*orig_stat)(const char *pathname, struct stat *statbuf);
static int hooked_stat(const char *pathname, struct stat *statbuf) {
    if (is_sensitive_path(pathname)) {
        errno = ENOENT;
        return -1;
    }
    return orig_stat(pathname, statbuf);
}

static int (*orig_open)(const char *pathname, int flags, mode_t mode);
static int hooked_open(const char *pathname, int flags, mode_t mode) {
    if (is_sensitive_path(pathname)) {
        errno = ENOENT;
        return -1;
    }
    return orig_open(pathname, flags, mode);
}

// ==========================================
// 3. UNAME SPOOFING (Samarkan Kernel Release dari Disclosure)
// ==========================================
static int (*orig_uname)(struct utsname *buf);
static int hooked_uname(struct utsname *buf) {
    int ret = orig_uname(buf);
    if (ret == 0 && buf != nullptr) {
        if (strstr(buf->release, "custom") != nullptr || 
            strstr(buf->release, "perf") != nullptr ||
            strstr(buf->version, "magisk") != nullptr) {
            strncpy(buf->release, "4.4.302-lineage-g", sizeof(buf->release) - 1);
        }
    }
    return ret;
}

// ==========================================
// 4. ZYGISK MODULE REGISTRATION
// ==========================================
class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
