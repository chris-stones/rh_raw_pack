
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(ffmpeg_INCLUDES)
#LOCAL_EXPORT_CFLAGS := -DRH_TARGET_ANDROID=1

LOCAL_MODULE    := rh_raw_loader
LOCAL_SRC_FILES := rh_raw_loader.c
LOCAL_SRC_FILES += rh_raw_avformat_wrapper.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

#LOCAL_STATIC_LIBRARIES := ffmpeg-prebuilt-avformat

#LOCAL_LDLIBS    += -lOpenSLES
#LOCAL_LDLIBS    += -llog
#LOCAL_LDLIBS    += -landroid

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

