LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	proto/pb_common.c \
	proto/pb_decode.c \
	proto/pb_encode.c \
	proto/sap-api.pb.c \
	sap-functions.c \
    secril-sap.c \
    secril-shim.c \
    sec-sap.c

LOCAL_SHARED_LIBRARIES := liblog libbinder
LOCAL_MODULE:= libsecril-shim
LOCAL_C_INCLUDES += proto
LOCAL_CFLAGS += -DPB_ENABLE_MALLOC -DPB_FIELD_16BIT

include $(BUILD_SHARED_LIBRARY)
