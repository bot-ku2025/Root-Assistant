#include <jni.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <android/log.h>
#include "zygisk.hpp"

#define LOG_TAG "RootAssistantDeepShield"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

JavaVM *g_jvm = nullptr;

// Thread siluman untuk meretas memori Java saat aplikasi berjalan
void* shield_thread(void* arg) {
    JNIEnv *env = nullptr;
    // Kaitkan thread C++ ke mesin Virtual Java (JVM)
    if (g_jvm->AttachCurrentThread(&env, nullptr) != JNI_OK) return nullptr;

    // Cari kelas AccessibilityManager di memori
    jclass accClass = env->FindClass("android/view/accessibility/AccessibilityManager");
    if (accClass != nullptr) {
        jfieldID mIsEnabledField = env->GetFieldID(accClass, "mIsEnabled", "Z");
        jfieldID sInstanceField = env->GetStaticFieldID(accClass, "sInstance", "Landroid/view/accessibility/AccessibilityManager;");

        // Polling loop: Paksa nilai Accessibility menjadi FALSE di dalam RAM aplikasi selama 10 detik awal
        for (int i = 0; i < 100; i++) { 
            if (sInstanceField != nullptr && mIsEnabledField != nullptr) {
                jobject sInstance = env->GetStaticObjectField(accClass, sInstanceField);
                if (sInstance != nullptr) {
                    // Eksekusi peretasan nilai boolean (FALSE)
                    env->SetBooleanField(sInstance, mIsEnabledField, JNI_FALSE);
                }
            }
            usleep(100000); // Tembak setiap 100 milidetik agar BCA tertipu
        }
    }
    
    g_jvm->DetachCurrentThread();
    return nullptr;
}

class RootAssistantHook : public zygisk::ModuleBase {
public:
    void onLoad(zygisk::Api *api, JNIEnv *env) override {
        this->api = api;
        env->GetJavaVM(&g_jvm);
    }

    void preAppSpecialize(zygisk::AppSpecializeArgs *args) override {
        api->setOption(zygisk::SHOULD_COMPATBLE_CONFIG);
    }

    void postAppSpecialize(const zygisk::AppSpecializeArgs *args) override {
        // Luncurkan thread siluman secara paralel saat aplikasi target dibuka
        pthread_t thread;
        pthread_create(&thread, nullptr, shield_thread, nullptr);
        pthread_detach(thread);
        LOGD("RootAssistantDeepShield: Memory overriding thread started in app process.");
    }

private:
    zygisk::Api *api;
};

REGISTER_ZYGISK_MODULE(RootAssistantHook)
