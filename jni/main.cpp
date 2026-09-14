#include <jni.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/mount.h>
#include <android/log.h>
#include <fstream>
#include <string>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantZygisk"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

class RootAssistantModule : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
        
        // Membaca sinyal Bridge Protocol dari post-fs-data.sh
        readBridgeEnv();
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        if (engine_state == "DISABLED") {
            // ReSukiSU Lock aktif / Modul Crash -> Batalkan hook
            api->setOption(zygisk::DLCLOSE_MODULE_LIBRARY);
            return;
        }

        // Jalankan Hiding hanya pada aplikasi (Child Process)
        if (engine_state == "NATIVE_ZYGISK") {
            // Mode Non-SusFS: Pemisahan Mount Namespace secara manual
            unshare(CLONE_NEWNS);
            umount2("/system/etc/hosts", MNT_DETACH);
            umount2("/data/adb", MNT_DETACH);
        }
    }

private:
    zygisk::Api *api;
    JNIEnv *env;
    std::string engine_state = "DISABLED";

    void readBridgeEnv() {
        std::ifstream envFile("/dev/root_assistant_env");
        if (!envFile.is_open()) return;

        std::string line;
        while (std::getline(envFile, line)) {
            if (line.rfind("ENGINE=", 0) == 0) {
                engine_state = line.substr(7);
            }
        }
        envFile.close();
        LOGI("Environment Engine Detected: %s", engine_state.c_str());
    }
};

REGISTER_ZYGISK_MODULE(RootAssistantModule)
