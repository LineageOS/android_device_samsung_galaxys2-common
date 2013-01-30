#!/bin/sh

# Copyright (C) 2013 The CyanogenMod Project
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

# This file is generated by device/common/generate-blob-scripts.sh - DO NOT EDIT

VENDOR=samsung
COMMON=galaxys2-common
COMMONOUTDIR=vendor/$VENDOR/$COMMON
COMMONBASE=../../../$COMMONOUTDIR/proprietary
COMMONMAKEFILE=../../../$COMMONOUTDIR/common-vendor-blobs.mk
COMMONPROPS=../galaxys2-common/proprietary-files.txt

mkdir -p ../../../vendor/$VENDOR/$COMMON/proprietary

adb root
adb wait-for-device

echo "Pulling common files..."
for FILE in `cat $COMMONPROPS | grep -v ^# | grep -v ^$`; do
    DIR=`dirname $FILE`
    if [ ! -d $COMMONBASE/$DIR ]; then
        mkdir -p $COMMONBASE/$DIR
    fi
    adb pull /$FILE $COMMONBASE/$FILE
done

(cat << EOF) | sed s/__COMMON__/$COMMON/g | sed s/__VENDOR__/$VENDOR/g > $COMMONMAKEFILE
# Copyright (C) 2012 The CyanogenMod Project
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

PRODUCT_PACKAGES += \\
    libTVOut \\
    libfimc \\
    libhdmi \\
    libhdmiclient \\
    libsecion
PRODUCT_COPY_FILES += \\
EOF

LINEEND=" \\"
COUNT=`cat $COMMONPROPS | grep -v ^# | grep -v ^$ | wc -l | awk {'print $1'}`
for FILE in `cat $COMMONPROPS | grep -v ^# | grep -v ^$`; do
    COUNT=`expr $COUNT - 1`
    if [ $COUNT = "0" ]; then
        LINEEND=""
    fi
    echo "    $COMMONOUTDIR/proprietary/$FILE:$FILE$LINEEND" >> $COMMONMAKEFILE
done

(cat << EOF) | sed s/__COMMON__/$COMMON/g | sed s/__VENDOR__/$VENDOR/g > $COMMONBASE/Android.mk
# Copyright (C) 2012 The CyanogenMod Project
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

LOCAL_PATH := \$(call my-dir)

ifneq (\$(filter i777 i9100 n7000,\$(TARGET_DEVICE)),)

include \$(CLEAR_VARS)
LOCAL_MODULE := libTVOut
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := system/lib/libTVOut.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := \$(TARGET_OUT)/lib
include \$(BUILD_PREBUILT)

include \$(CLEAR_VARS)
LOCAL_MODULE := libsecion
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := system/lib/libsecion.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := \$(TARGET_OUT)/lib
include \$(BUILD_PREBUILT)

include \$(CLEAR_VARS)
LOCAL_MODULE := libfimc
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := system/lib/libfimc.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := \$(TARGET_OUT)/lib
include \$(BUILD_PREBUILT)

include \$(CLEAR_VARS)
LOCAL_MODULE := libhdmi
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := system/lib/libhdmi.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := \$(TARGET_OUT)/lib
include \$(BUILD_PREBUILT)

include \$(CLEAR_VARS)
LOCAL_MODULE := libhdmiclient
LOCAL_MODULE_OWNER := samsung
LOCAL_SRC_FILES := system/lib/libhdmiclient.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_PATH := \$(TARGET_OUT)/lib
include \$(BUILD_PREBUILT)

endif

EOF

(cat << EOF) | sed s/__COMMON__/$COMMON/g | sed s/__VENDOR__/$VENDOR/g > ../../../$COMMONOUTDIR/common-vendor.mk
# Copyright (C) 2012 The CyanogenMod Project
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

# Pick up overlay for features that depend on non-open-source files
DEVICE_PACKAGE_OVERLAYS += vendor/__VENDOR__/__COMMON__/overlay

\$(call inherit-product, vendor/__VENDOR__/__COMMON__/common-vendor-blobs.mk)
EOF
