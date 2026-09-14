LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := root-assistant
LOCAL_SRC_FILES := main.cpp
LOCAL_LDLIBS := -llog -landroid
LOCAL_CPPFLAGS := -std=c++17 -O3 -fvisibility=hidden
LOCAL_CFLAGS := -O3 -fvisibility=hidden
include $(BUILD_SHARED_LIBRARY)
