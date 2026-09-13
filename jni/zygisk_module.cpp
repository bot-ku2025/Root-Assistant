#include <unistd.h>
#include <fcntl.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "zygisk.hpp"

using zygisk::Api;
using zygisk::AppSpecializeArgs;

class RootAssistantZygisk : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        if (!args || !args->nice_name) return;

        const char *process_name = env->GetStringUTFChars(args->nice_name, nullptr);
        if (process_name && strstr(process_name, "com.android.shell") == nullptr) {
            ptrace(PTRACE_DETACH, 0, NULL, NULL);
        }

        if (process_name) {
            env->ReleaseStringUTFChars(args->nice_name, process_name);
        }
    }

private:
    Api *api;
    JNIEnv *env;
};

REGISTER_ZYGISK_MODULE(RootAssistantZygisk)
