#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdint.h>
#include <cstring>

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

// Fungsi pipa pengiriman data terhubung ke Companion daemon (menggunakan casting void* ke int)
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

class RootAssistantPhase5Interconnected : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        
        // 1. Gatekeeper Lingkungan (ResuKisu + SuSFS / KSU Next) sebagai gerbang mutlak
        if (!validateEnvironment()) {
            LOGE("[ABORT] Unsupported environment! Root-Assistant requires ResuKisu+SuSFS or valid KSU Next.");
            return;
        }
        
        LOGI("RootAssistant Phase 5 (Interconnected IPC Pipeline) loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    // 2. Filter Target Aplikasi menyalurkan data langsung ke pipa IPC
                    if (strstr(name, "com.android") == nullptr) {
                        LOGD("Pipeline Target App Detected -> %s", name);
                        
                        // 3. Mengumpankan nama paket langsung ke daemon companion secara real-time
                        sendDataToCompanion(api, name);
                    }
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured under interconnected pipeline.");
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

// 4. Handler Companion Daemon yang menerima data terhubung dari pipa aplikasi
static void companion_handler(int socket_fd) {
    uint32_t len = 0;
    if (read(socket_fd, &len, sizeof(len)) == sizeof(len) && len > 0 && len < 256) {
        char package_name[256];
        memset(package_name, 0, sizeof(package_name));
        if (read(socket_fd, package_name, len) > 0) {
            LOGI("[DAEMON PIPELINE] Successfully received target package through IPC: %s", package_name);
        }
    }
    close(socket_fd);
}

static void register_module(zygisk::Api *api) {
    zygisk::ModuleBase *module = new RootAssistantPhase5Interconnected();
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
