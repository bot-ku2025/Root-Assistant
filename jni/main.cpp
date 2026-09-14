#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <sys/system_properties.h>

#define LOG_TAG "RootAssistant"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Definisi Standar Zygisk API langsung (Self-contained implementation)
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
    enum Option {
        PATCH_DLOPEN = 0,
        DLCLOSE_SKIP_FINI = 1
    };
    virtual void *pltHookRegister(const char *lib, const char *symbol, void *new_func, void **old_func) = 0;
    virtual void *pltHookCommit() = 0;
    virtual void setOption(Option opt) = 0;
    virtual void *connectCompanion() = 0;
    virtual void registerModule(ModuleBase *module) = 0;
    static void init(Api *api, JNIEnv *env) {}
};
} // namespace zygisk

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        LOGI("RootAssistant Self-Contained Core loaded successfully.");
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->nice_name) {
            jstring nice_name = *args->nice_name;
            if (nice_name && env) {
                const char *name = env->GetStringUTFChars(nice_name, nullptr);
                if (name) {
                    LOGD("Target App Detected -> %s", name);
                    env->ReleaseStringUTFChars(nice_name, name);
                }
            }
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        LOGI("System server specialization secured.");
    }

private:
    zygisk::Api *api = nullptr;
    JNIEnv *env = nullptr;
};

// Ekspor entry point Zygisk secara mandiri
static void register_module(zygisk::Api *api) {
    zygisk::ModuleBase *module = new RootAssistantModule();
    api->registerModule(module);
}

extern "C" {
__attribute__((visibility("default"))) __attribute__((used))
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) {
    zygisk::Api::init(api, env);
    register_module(api);
}
}
