LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rootassistant
LOCAL_SRC_FILES := main.cpp
LOCAL_CPPFLAGS := -std=c++17 -Wall -O3
LOCAL_LDLIBS := -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)
include $(BUILD_SHARED_LIBRARY)
