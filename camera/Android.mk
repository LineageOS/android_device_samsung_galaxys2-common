#
# Copyright (C) 2013 Paul Kocialkowski
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	exynos_camera.c \
	exynos_exif.c \
	exynos_param.c \
	exynos_v4l2.c

LOCAL_C_INCLUDES := \
	system/media/camera/include \
	hardware/samsung/exynos4/hal/include

LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libcamera_client libhardware libs5pjpeg
LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := camera.$(TARGET_BOOTLOADER_BOARD_NAME)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
