# In order to compile your application under cygwin
# you might need to define NDK_USE_CYGPATH=1 before calling the ndk-build

USER_LOCAL_PATH:=$(LOCAL_PATH)

USER_LOCAL_C_INCLUDES:=$(LOCAL_C_INCLUDES)
USER_LOCAL_CFLAGS:=$(LOCAL_CFLAGS)
USER_LOCAL_STATIC_LIBRARIES:=$(LOCAL_STATIC_LIBRARIES)
USER_LOCAL_SHARED_LIBRARIES:=$(LOCAL_SHARED_LIBRARIES)
USER_LOCAL_LDLIBS:=$(LOCAL_LDLIBS)

LOCAL_PATH:=$(subst ?,,$(firstword ?$(subst \, ,$(subst /, ,$(call my-dir)))))

OPENCV_TARGET_ARCH_ABI:=$(TARGET_ARCH_ABI)
OPENCV_THIS_DIR:=$(patsubst $(LOCAL_PATH)\\%,%,$(patsubst $(LOCAL_PATH)/%,%,$(call my-dir)))
OPENCV_MK_DIR:=$(dir $(lastword $(MAKEFILE_LIST)))
OPENCV_3RDPARTY_LIBS_DIR:=$(OPENCV_THIS_DIR)/../3rdparty/libs/$(OPENCV_TARGET_ARCH_ABI)
OPENCV_BASEDIR:=
OPENCV_LOCAL_C_INCLUDES:="$(LOCAL_PATH)/$(OPENCV_THIS_DIR)/include/opencv" "$(LOCAL_PATH)/$(OPENCV_THIS_DIR)/include"
OPENCV_MODULES:=shape ml dnn objdetect superres stitching videostab calib3d features2d highgui videoio imgcodecs video photo imgproc flann core
OPENCV_SUB_MK:=$(call my-dir)/OpenCV-$(TARGET_ARCH_ABI).mk

ifeq ($(OPENCV_LIB_TYPE),)
    OPENCV_LIB_TYPE:=SHARED
endif

ifeq ($(OPENCV_LIB_TYPE),SHARED)
    OPENCV_LIBS:=java3
    OPENCV_LIB_TYPE:=SHARED
else
    OPENCV_LIBS:=$(OPENCV_MODULES)
    OPENCV_LIB_TYPE:=STATIC
endif

ifeq ($(OPENCV_LIB_TYPE),SHARED)
    OPENCV_3RDPARTY_COMPONENTS:=
    OPENCV_EXTRA_COMPONENTS:=
else
    include $(OPENCV_SUB_MK)
endif

ifeq ($(OPENCV_LIB_TYPE),SHARED)
    OPENCV_LIBS_DIR:=$(OPENCV_THIS_DIR)/../libs/$(OPENCV_TARGET_ARCH_ABI)
    OPENCV_LIB_SUFFIX:=so
else
    OPENCV_LIBS_DIR:=$(OPENCV_THIS_DIR)/../staticlibs/$(OPENCV_TARGET_ARCH_ABI)
    OPENCV_LIB_SUFFIX:=a
    OPENCV_INSTALL_MODULES:=on
endif

define add_opencv_module
    include $(CLEAR_VARS)
    LOCAL_MODULE:=opencv_$1
    LOCAL_SRC_FILES:=$(OPENCV_LIBS_DIR)/libopencv_$1.$(OPENCV_LIB_SUFFIX)
    include $(PREBUILT_$(OPENCV_LIB_TYPE)_LIBRARY)
endef

define add_opencv_3rdparty_component
    include $(CLEAR_VARS)
    LOCAL_MODULE:=$1
    LOCAL_SRC_FILES:=$(OPENCV_3RDPARTY_LIBS_DIR)/lib$1.a
    include $(PREBUILT_STATIC_LIBRARY)
endef

ifeq ($(OPENCV_MK_$(OPENCV_TARGET_ARCH_ABI)_ALREADY_INCLUDED),)
    ifeq ($(OPENCV_INSTALL_MODULES),on)
        $(foreach module,$(OPENCV_LIBS),$(eval $(call add_opencv_module,$(module))))
    endif

    $(foreach module,$(OPENCV_3RDPARTY_COMPONENTS),$(eval $(call add_opencv_3rdparty_component,$(module))))

    ifneq ($(OPENCV_BASEDIR),)
        OPENCV_LOCAL_C_INCLUDES += $(foreach mod, $(OPENCV_MODULES), $(OPENCV_BASEDIR)/modules/$(mod)/include)
    endif

    #turn off module installation to prevent their redefinition
    OPENCV_MK_$(OPENCV_TARGET_ARCH_ABI)_ALREADY_INCLUDED:=on
endif

ifeq ($(OPENCV_LOCAL_CFLAGS),)
    OPENCV_LOCAL_CFLAGS := -fPIC -DANDROID -fsigned-char
endif

include $(CLEAR_VARS)

LOCAL_C_INCLUDES:=$(USER_LOCAL_C_INCLUDES)
LOCAL_CFLAGS:=$(USER_LOCAL_CFLAGS)
LOCAL_STATIC_LIBRARIES:=$(USER_LOCAL_STATIC_LIBRARIES)
LOCAL_SHARED_LIBRARIES:=$(USER_LOCAL_SHARED_LIBRARIES)
LOCAL_LDLIBS:=$(USER_LOCAL_LDLIBS)

# Details: #10229
ifeq ($(OPENCV_SKIP_ANDROID_IPP_FIX_1),)
  LOCAL_LDFLAGS += -Wl,--exclude-libs,libippicv.a
  LOCAL_LDFLAGS += -Wl,--exclude-libs,libippiw.a
else
  ifeq ($(OPENCV_SKIP_ANDROID_IPP_FIX_2),)
    LOCAL_LDFLAGS += -Wl,-Bsymbolic
  endif
endif

LOCAL_C_INCLUDES += $(OPENCV_LOCAL_C_INCLUDES)
LOCAL_CFLAGS     += $(OPENCV_LOCAL_CFLAGS)

ifeq ($(OPENCV_INSTALL_MODULES),on)
    LOCAL_$(OPENCV_LIB_TYPE)_LIBRARIES += $(foreach mod, $(OPENCV_LIBS), opencv_$(mod))
else
    $(call __ndk_info,OpenCV: You should ignore warning about 'non-system libraries in linker flags' and 'opencv_java' library.)
    $(call __ndk_info,        'OPENCV_INSTALL_MODULES:=on' can be used to build APK with included OpenCV binaries)
    LOCAL_LDLIBS += -L$(call host-path,$(LOCAL_PATH)/$(OPENCV_LIBS_DIR)) $(foreach lib, $(OPENCV_LIBS), -lopencv_$(lib))
endif

ifeq ($(OPENCV_LIB_TYPE),STATIC)
    LOCAL_STATIC_LIBRARIES += $(OPENCV_3RDPARTY_COMPONENTS)
endif

LOCAL_LDLIBS += $(foreach lib,$(OPENCV_EXTRA_COMPONENTS), -l$(lib))

#restore the LOCAL_PATH
LOCAL_PATH:=$(USER_LOCAL_PATH)
