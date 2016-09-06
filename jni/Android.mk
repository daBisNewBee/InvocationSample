LOCAL_PATH := $(call my-dir)
MY_SOPATH := $(LOCAL_PATH)/../libs/armeabi

include $(CLEAR_VARS)
LOCAL_LDLIBS :=	-L$(SYSROOT)/usr/lib		\
				-llog

LOCAL_C_INCLUDES := MyInvocation.h

LOCAL_MODULE    := MyInvocation
LOCAL_SRC_FILES := MyInvocation.cpp

include $(BUILD_SHARED_LIBRARY)

#-----------------------------------------fakeVPN-----------------------------------------#
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := MyInvocation.h

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib	\
				-L$(MY_SOPATH)			\
				-llog					\
#				-lfilecert_ex

LOCAL_MODULE    := fakeVPN
LOCAL_SRC_FILES := fakeVPN.c
include $(BUILD_SHARED_LIBRARY)
#-----------------------------------------keyStore文件证书-----------------------------------------#
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := MyInvocation.h

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib	\
				-L$(MY_SOPATH)			\
				-llog					\
				-lMyInvocation

LOCAL_MODULE    := filecert_ex
LOCAL_SRC_FILES := filecert_ex.cpp
include $(BUILD_SHARED_LIBRARY)





