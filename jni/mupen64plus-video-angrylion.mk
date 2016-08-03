######################
# mupen64plus-rsp-angrylion
######################
include $(CLEAR_VARS)
LOCAL_PATH := $(JNI_LOCAL_PATH)
SRCDIR := mupen64plus-video-angrylion

MY_LOCAL_CFLAGS := $(COMMON_CFLAGS) -DM64P_PLUGIN_API

LOCAL_MODULE := mupen64plus-video-angrylion
LOCAL_ARM_MODE := arm
LOCAL_C_INCLUDES := $(M64P_API_INCLUDES) $(LIBRETRO_INCLUDES)/libretro-common/include $(LIBRETRO_INCLUDES)

ifeq ($(TARGET_ARCH_ABI), x86)
    MY_LOCAL_CFLAGS += -DUSE_SSE_SUPPORT
endif

LOCAL_SRC_FILES := \
    $(SRCDIR)/n64video.c \
    $(SRCDIR)/n64video_main.c \
    $(SRCDIR)/n64video_vi.c \
    $(SRCDIR)/osal_dynamiclib_unix.c \
    ../$(LIBRETRO_INCLUDES)/libretro_custom.c \
    ../$(LIBRETRO_INCLUDES)/Graphics/plugins.c

LOCAL_CFLAGS := $(MY_LOCAL_CFLAGS)
LOCAL_CPPFLAGS := $(COMMON_CPPFLAGS)
LOCAL_LDLIBS := -llog

include $(BUILD_SHARED_LIBRARY)
