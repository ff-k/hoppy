LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := MEngine
LOCAL_SRC_FILES := android_mengine.c
LOCAL_LDLIBS := -landroid -llog -lEGL -lGLESv2
LOCAL_CFLAGS := -g -DSW_RENDERER=1
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
