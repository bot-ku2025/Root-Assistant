#pragma once
#include <jni.h>
#include <stdint.h>

namespace zygisk {
struct AppSpecializeArgs;
struct ServerSpecializeArgs;
class Api;

class ModuleBase {
public:
    virtual void onLoad(Api *api, JNIEnv *env) {}
    virtual void preAppSpecialize(AppSpecializeArgs *args) {}
    virtual void postAppSpecialize(const AppSpecializeArgs *args) {}
    virtual void preServerSpecialize(ServerSpecializeArgs *args) {}
    virtual void postServerSpecialize(const ServerSpecializeArgs *args) {}
};

enum Option : uint32_t {
    FORCE_DENYLIST_UNMOUNT = 0,
    DLCLOSE_MODULE_LIBRARY = 1,
};

class Api {
public:
    virtual bool setOption(Option option) = 0;
};
}

#define REGISTER_ZYGISK_MODULE(className) \
extern "C" [[gnu::visibility("default")]] \
void zygisk_module_entry(zygisk::Api *api, JNIEnv *env) { \
    static className module; \
    module.onLoad(api, env); \
}
