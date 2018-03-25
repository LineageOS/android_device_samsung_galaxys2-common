# Copyright (C) 2017 The LineageOS Project
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
LOCAL_SRC_FILES := bionic.cpp
LOCAL_MODULE := libc-shim
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_32_BIT_ONLY := true
LOCAL_SHARED_LIBRARIES := \
	libc \
	liblog

LOCAL_C_INCLUDES := \
    $(TOP)/bionic/libc \
    $(TOP)/bionic/libc/include \
    $(TOP)/bionic/libc/bionic \
    $(TOP)/bionic/libc/async_safe/include

include $(BUILD_SHARED_LIBRARY)
