#include <sys/types.h>
#include "zygisk.hpp"

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (args && args->uid >= 10000) {
            // SENJATA MUTLAK: Hapus modul Root-Assistant dari RAM seketika.
            // Tidak ada LOGI agar tidak meninggalkan jejak di logcat (stealth).
            api->setOption(zygisk::Option::DLCLOSE_MODULE_LIBRARY);
        }
    }

    // FUNGSI postAppSpecialize DIHAPUS TOTAL!
    // Membiarkan fungsi ini ada saat DLCLOSE aktif adalah penyebab crash murni.

private:
    zygisk::Api *api;
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
