# Copyright (C) 2016 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CLANG := true
LOCAL_SRC_FILES := \
    bootloader_message.cpp \
    roots.cpp \
    mounts.c \
    mtdutils.c \
    ../../../../../external/e2fsprogs/lib/blkid/resolve.c \
    ../../../../../external/e2fsprogs/lib/blkid/cache.c \
    ../../../../../external/e2fsprogs/lib/blkid/tag.c \
    ../../../../../external/e2fsprogs/lib/blkid/getsize.c \
    ../../../../../external/e2fsprogs/lib/blkid/save.c \
    ../../../../../external/e2fsprogs/lib/blkid/read.c \
    ../../../../../external/e2fsprogs/lib/blkid/dev.c \
    ../../../../../external/e2fsprogs/lib/blkid/probe.c \
    ../../../../../external/e2fsprogs/lib/blkid/devname.c \
    ../../../../../external/e2fsprogs/lib/blkid/devno.c \
    ../../../../../external/e2fsprogs/lib/blkid/llseek.c \
    ../../../../../external/e2fsprogs/lib/blkid/probe_exfat.c \
    ../../../../../external/e2fsprogs/lib/blkid/version.c \
    ../../../../../external/e2fsprogs/lib/uuid/clear.c \
    ../../../../../external/e2fsprogs/lib/uuid/compare.c \
    ../../../../../external/e2fsprogs/lib/uuid/copy.c \
    ../../../../../external/e2fsprogs/lib/uuid/gen_uuid.c \
    ../../../../../external/e2fsprogs/lib/uuid/isnull.c \
    ../../../../../external/e2fsprogs/lib/uuid/pack.c \
    ../../../../../external/e2fsprogs/lib/uuid/parse.c \
    ../../../../../external/e2fsprogs/lib/uuid/unpack.c \
    ../../../../../external/e2fsprogs/lib/uuid/unparse.c \
    ../../../../../external/e2fsprogs/lib/uuid/uuid_time.c


LOCAL_MODULE := libbootloader_message
LOCAL_SHARED_LIBRARIES := libbase liblog
LOCAL_STATIC_LIBRARIES := libfs_mgr
LOCAL_CFLAGS := -O2 -g -W -Wall -fno-strict-aliasing \
	-DHAVE_UNISTD_H \
	-DHAVE_ERRNO_H \
	-DHAVE_NETINET_IN_H \
	-DHAVE_SYS_IOCTL_H \
	-DHAVE_SYS_MMAN_H \
	-DHAVE_SYS_MOUNT_H \
	-DHAVE_SYS_RESOURCE_H \
	-DHAVE_SYS_SELECT_H \
	-DHAVE_SYS_STAT_H \
	-DHAVE_SYS_TYPES_H \
	-DHAVE_STDLIB_H \
	-DHAVE_STRDUP \
	-DHAVE_MMAP \
	-DHAVE_UTIME_H \
	-DHAVE_GETPAGESIZE \
	-DHAVE_EXT2_IOCTLS \
	-DHAVE_TYPE_SSIZE_T \
	-DHAVE_SYS_TIME_H \
	-DHAVE_SYS_PARAM_H \
	-DHAVE_SYSCONF \
	-DHAVE_LINUX_FD_H \
	-DHAVE_SYS_PRCTL_H \
	-DHAVE_LSEEK64 \
	-DHAVE_LSEEK64_PROTOTYPE

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
    system/vold \
    system/extras/ext4_utils \
    system/core/adb \
    external/e2fsprogs/lib

LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_PATH)/include
include $(BUILD_STATIC_LIBRARY)
