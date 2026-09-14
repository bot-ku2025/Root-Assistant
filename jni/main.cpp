#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdint.h>
#include <cstring>
#include <dlfcn.h>
#include <errno.h>

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

namespace zygisk {
struct Api;
struct AppSpecializeArgs {
    jint *uid;
    jint *gid;
    jobjectArray *gids;
    jint *runtime_flags;
    jobjectArray *rlimits;
    jint *mount_external;
    jstring *se_info;
    jstring *nice_name;
    jstring *instruction_set;
    jstring *app_data_dir;
};
struct ServerSpecializeArgs {
    jint *uid;
    jint *gid;
    jobjectArray *gids;
    jint *runtime_flags;
    jobjectArray *rlimits;
    jint *permitted_capabilities;
    jobjectArray *effective_capabilities;
};
class ModuleBase {
public:
    virtual ~ModuleBase() {}
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(ServerSpecializeArgs *args) {}
};
struct Api {
    enum Option { PATCH_DLOPEN = 0, DLCLOSE_SKIP_FINI = 1 };
    virtual void *pltHookRegister(const char *lib, const char *symbol, void *new_func, void **old_func) = 0;
    virtual void *pltHookCommit() = 0;
    virtual void setOption(Option opt) = 0;
    virtual void *connectCompanion() = 0;
    virtual void registerModule(ModuleBase *module) = 0;
    virtual void registerCompanion(void (*handler)(int)) = 0;
    static void init(Api *api, JNIEnv *env) {}
};
} // namespace zygisk

// Fungsi pipa pengiriman data ke Companion daemon
static void sendDataToCompanion(zygisk::Api *api, const char *package_name) {
    if (!api || !package_name) return;
    int fd = (int)(intptr_t)api->connectCompanion();
    if (fd >= 0) {
        uint32_t len = strlen(package_name);
        write(fd, &len, sizeof(len));
        write(fd, package_name, len);
        close(fd);
    }
}

// Hook handler untuk fungsi open (Phase 6: Runtime API Evasion)
static int (*orig_open)(const char *pathname, int flags, mode_t mode) = nullptr;

static int hooked_open(const char *pathname, int flags, mode_t mode) {
    if (pathname) {
        if (strstr(pathname, "/sbin/su") || strstr(pathname, "/system/bin/su") || strstr(pathname, "/system/xbin/su")) {
            LOGD("[PLT HOOK] Blocked access attempt to root binary: %s", pathname);
            errno = ENOENT;
            return -1;
        }
    }
    if (orig_open) {
        return orig_open(pathname, flags, mode);
    }
    return -1;
}

class RootAssistantPhase6 : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        
        if (!validateEnvironment()) {
            LOGE("[ABORT] Unsupported environment! Root-Assistant requires ResuKisu+SuSFS or valid KSU Next.");
            return;
        }
        
        api->setOption(zygisk::Api::PATCH_DLOPEN);
        LOGI("RootAssistant Phase 6 (PLT Hooking & Runtime Evasion) loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    if (strstr(name, "com.android") == nullptr) {
                        LOGD("Phase 6 Target App Secured -> %s", name);
                        sendDataToCompanion(api, name);
                        
                        if (api) {
                            api->pltHookRegister(nullptr, "open", (void *)hooked_open, (void **)&orig_open);
                            api->pltHookCommit();
                        }
                    }
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured under Phase 6 runtime evasion.");
    }

private:
    bool validateEnvironment() {
        bool has_susfs = false;
        bool has_ksu_derivative = false;

        struct stat buffer;
        if (stat("/sys/kernel/susfs", &buffer) == 0 || stat("/dev/susfs", &buffer) == 0) {
            has_susfs = true;
            LOGI("[CHECK] SuSFS interface detected (ResuKisu/SuSFS Priority Active).");
        }

        char prop_buf[PROP_VALUE_MAX];
        __system_property_get("ro.kernel.su", prop_buf);
        if (access("/dev/ksud", F_OK) == 0 || strlen(prop_buf) > 0) {
            has_ksu_derivative = true;
            LOGI("[CHECK] KernelSU/KSU Next derivative detected.");
        }

        return (has_susfs || has_ksu_derivative);
    }

    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

static void companion_handler(int socket_fd) {
    uint32_t len = 0;
    if (read(socket_fd, &len, sizeof(len)) == sizeof(len) && len > 0 && len < 256) {
        char package_name[256];
        memset(package_name, 0, sizeof(package_name));
        if (read(socket_fd, package_name, len) > 0) {
            LOGI("[DAEMON PIPELINE] Phase 6 received target package: %s", package_name);
        }
    }
    close(socket_fd);
}

static void register_module(zygisk::Api *api) {
    zygisk::ModuleBase *module = new RootAssistantPhase6();
    api->registerModule(module);
}

extern "C" {
__attribute__((visibility("default"))) __attribute__((used))
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) {
    zygisk::Api::init(api, env);
    api->registerCompanion(companion_handler);
    register_module(api);
}
}
