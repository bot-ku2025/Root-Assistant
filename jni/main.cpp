#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include <sys/stat.h>
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
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};
struct Api {
    enum Option { PATCH_DLOPEN = 0, DLCLOSE_SKIP_FINI = 1 };
    virtual void *pltHookRegister(const char *lib, const char *symbol, void *new_func, void **old_func) = 0;
    virtual void *pltHookCommit() = 0;
    virtual void setOption(Option opt) = 0;
    virtual void *connectCompanion() = 0;
    virtual void registerModule(ModuleBase *module) = 0;
    static void init(Api *api, JNIEnv *env) {}
};
} // namespace zygisk

class RootAssistantPhase4_5 : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        
        if (!validateEnvironment()) {
            LOGE("[ABORT] Unsupported environment detected! Root-Assistant requires ResuKisu+SuSFS or valid KSU Next.");
            return;
        }
        
        LOGI("RootAssistant Phase 4.5 (Environment Gatekeeper) loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    if (strstr(name, "com.android") == nullptr) {
                        LOGD("Verified Target App -> %s", name);
                    }
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured under verified environment.");
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

        if (has_susfs || has_ksu_derivative) {
            return true;
        }

        return false;
    }

    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

static void register_module(zygisk::Api *api) {
    zygisk::ModuleBase *module = new RootAssistantPhase4_5();
    api->registerModule(module);
}

extern "C" {
__attribute__((visibility("default"))) __attribute__((used))
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) {
    zygisk::Api::init(api, env);
    register_module(api);
}
}
