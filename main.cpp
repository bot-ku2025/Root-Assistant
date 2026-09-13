#include <jni.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include "zygisk.hpp"

// ==========================================
// 1. PROPERTY SANITIZATION (Cloaking Sentinel)
// ==========================================
static int (*orig___system_property_get)(const char *name, char *value);
static int hooked___system_property_get(const char *name, char *value) {
    if (name != nullptr) {
        if (strncmp(name, "persist.sys.sentinel.", 21) == 0 ||
            strstr(name, "magisk") != nullptr ||
            strstr(name, "kernelsu") != nullptr ||
            strstr(name, "ksu") != nullptr) {
            if (value != nullptr) {
                value[0] = '\0';
            }
            return 0;
        }
    }
    return orig___system_property_get(name, value);
}

// ==========================================
// 2. VFS DIRECTORY CLOAKING (/data/adb/*)
// ==========================================
static struct dirent* (*orig_readdir)(DIR *dirp);
static struct dirent* hooked_readdir(DIR *dirp) {
    struct dirent* entry = orig_readdir(dirp);
    while (entry != nullptr) {
        if (strcmp(entry->d_name, "ModuleSentinel") == 0 ||
            strcmp(entry->d_name, "lspd") == 0 ||
            strcmp(entry->d_name, "tricky_store") == 0 ||
            strcmp(entry->d_name, "zygisksu") == 0) {
            entry = orig_readdir(dirp);
            continue;
        }
        break;
    }
    return entry;
}

// ==========================================
// 3. PATH ACCESS & STAT CLOAKING
// ==========================================
static int (*orig_access)(const char *pathname, int mode);
static int hooked_access(const char *pathname, int mode) {
    if (pathname != nullptr) {
        if (strstr(pathname, "ModuleSentinel") != nullptr ||
            strstr(pathname, "lspd") != nullptr ||
            strstr(pathname, "tricky_store") != nullptr ||
            strstr(pathname, "zygisksu") != nullptr) {
            errno = ENOENT;
            return -1;
        }
    }
    return orig_access(pathname, mode);
}

static int (*orig_stat)(const char *pathname, struct stat *statbuf);
static int hooked_stat(const char *pathname, struct stat *statbuf) {
    if (pathname != nullptr) {
        if (strstr(pathname, "ModuleSentinel") != nullptr ||
            strstr(pathname, "lspd") != nullptr ||
            strstr(pathname, "tricky_store") != nullptr ||
            strstr(pathname, "zygisksu") != nullptr) {
            errno = ENOENT;
            return -1;
        }
    }
    return orig_stat(pathname, statbuf);
}

// ==========================================
// 4. ZYGISK ENTRY & HOOK REGISTRATION
// ==========================================
class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        // Pasang hook di sini sesuai framework hook yang digunakan (misal: Dobby / PLT)
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
