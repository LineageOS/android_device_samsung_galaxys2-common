LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := \
	proto/pb_common.cpp \
	proto/pb_decode.cpp \
	proto/pb_encode.cpp \
	proto/sap-api.pb.cpp \
	sap-functions.cpp \
	secril-sap.cpp \
	sec-sap.cpp \
	secril-shim.cpp

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libril \
	libcutils \
	libbinder \
	libc

LOCAL_C_INCLUDES += proto
LOCAL_CFLAGS := -Wall -Werror -DPB_ENABLE_MALLOC -Wno-unused-parameter

LOCAL_MODULE := libsecril-shim
LOCAL_VENDOR_MODULE := true

include $(BUILD_SHARED_LIBRARY)
