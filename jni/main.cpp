#include <sys/types.h>
#include "zygisk.hpp"

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->uid >= 10000) {
            // Bersihkan memori aplikasi dari jejak modul seketika tanpa log/crash
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

private:
    zygisk::Api *api;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
