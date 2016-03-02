LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := NDKOpenGLBackDraw
LOCAL_SRC_FILES := NDKOpenGLBackDraw.c

LOCAL_LDLIBS := -llog -lGLESv2 -landroid -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
