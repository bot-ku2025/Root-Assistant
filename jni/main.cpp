#include <sys/types.h>
#include <android/log.h>
#define RA_LOG(...) __android_log_print(ANDROID_LOG_INFO,"RootAssistant",__VA_ARGS__)
#include <cstdint>
#include <android/log.h>
#define RA_LOG(...) __android_log_print(ANDROID_LOG_INFO, "RootAssistant", __VA_ARGS__)
#include "zygisk.hpp"

namespace ra {
struct EngineState { bool initialized=false; bool platform_supported=false; bool capability_ready=false; };
static EngineState state{};
static bool initialize_foundation() { state.initialized=true; return true; }
}


namespace ra {
struct EngineState {
    bool initialized = false;
    bool platform_supported = false;
    bool capability_ready = false;
};

static EngineState state{};

static bool initialize_foundation() {
    state.initialized = true;
    return true;
}
}

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        if (!ra::initialize_foundation()) {
            RA_LOG("Root Assistant foundation initialization failed; engine remains disabled");
            return;
        }
        RA_LOG("Root Assistant runtime initialized");
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        if (!api || !args) return;
        api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (!api || !args)
            return;

        if (args->uid >= 10000) {
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

    void preServerSpecialize(zygisk::ServerSpecializeArgs *args) override {
        if (!api || !args) return;
        RA_LOG("Root Assistant server runtime initialized");
    }

private:
    zygisk::Api *api = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
