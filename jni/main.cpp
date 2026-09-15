#include <sys/types.h>
#include "zygisk.hpp"

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (!api || !args)
            return;

        if (args->uid >= 10000) {
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

private:
    zygisk::Api *api = nullptr;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
