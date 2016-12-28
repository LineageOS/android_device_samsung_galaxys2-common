/*
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2014 The CyanogenMod Project
 * Copyright (C) 2014-2015 Andreas Schneider <asn@cryptomilk.org>
 * Copyright (C) 2014-2015 Christopher N. Hesse <raymanfx@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

#define LOG_TAG "SamsungPowerHAL"
/* #define LOG_NDEBUG 0 */
#include <utils/Log.h>

#include <hardware/hardware.h>
#include <hardware/power.h>

#include "power.h"

struct samsung_power_module {
    struct power_module base;
};

enum power_profile_e {
    PROFILE_POWER_SAVE = 0,
    PROFILE_BALANCED,
    PROFILE_HIGH_PERFORMANCE
};
static enum power_profile_e current_power_profile = PROFILE_BALANCED;

/**********************************************************
 *** HELPER FUNCTIONS
 **********************************************************/

static int sysfs_read(char *path, char *s, int num_bytes)
{
    char errno_str[64];
    int len;
    int ret = 0;
    int fd;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        strerror_r(errno, errno_str, sizeof(errno_str));
        ALOGE("Error opening %s: %s\n", path, errno_str);

        return -1;
    }

    len = read(fd, s, num_bytes - 1);
    if (len < 0) {
        strerror_r(errno, errno_str, sizeof(errno_str));
        ALOGE("Error reading from %s: %s\n", path, errno_str);

        ret = -1;
    } else {
        s[len] = '\0';
    }

    close(fd);

    return ret;
}

static void sysfs_write(const char *path, char *s)
{
    char errno_str[64];
    int len;
    int fd;

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        strerror_r(errno, errno_str, sizeof(errno_str));
        ALOGE("Error opening %s: %s\n", path, errno_str);
        return;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, errno_str, sizeof(errno_str));
        ALOGE("Error writing to %s: %s\n", path, errno_str);
    }

    close(fd);
}

/**********************************************************
 *** POWER FUNCTIONS
 **********************************************************/

static void set_power_profile(struct samsung_power_module *samsung_pwr,
                              enum power_profile_e profile)
{
    int rc;
    struct stat sb;

    if (current_power_profile == profile) {
        return;
    }

    ALOGV("%s: profile=%d", __func__, profile);

    switch (profile) {
        case PROFILE_POWER_SAVE:
            sysfs_write(GOVERNOR_PATH, GOV_POWERSAVE);
            ALOGD("%s: set powersave mode", __func__);
            break;
        case PROFILE_BALANCED:
            sysfs_write(GOVERNOR_PATH, GOV_BALANCED);
            ALOGD("%s: set balanced mode", __func__);
            break;
        case PROFILE_HIGH_PERFORMANCE:
            sysfs_write(GOVERNOR_PATH, GOV_HIGH_PERFORMANCE);
            ALOGD("%s: set performance mode", __func__);
            break;
    }

    current_power_profile = profile;
}


static void samsung_power_init(struct power_module *module)
{
}

/*
 * The powerHint function is called to pass hints on power requirements, which
 * may result in adjustment of power/performance parameters of the cpufreq
 * governor and other controls.
 *
 * The possible hints are:
 *
 * POWER_HINT_VSYNC
 *
 *     Foreground app has started or stopped requesting a VSYNC pulse
 *     from SurfaceFlinger.  If the app has started requesting VSYNC
 *     then CPU and GPU load is expected soon, and it may be appropriate
 *     to raise speeds of CPU, memory bus, etc.  The data parameter is
 *     non-zero to indicate VSYNC pulse is now requested, or zero for
 *     VSYNC pulse no longer requested.
 *
 * POWER_HINT_INTERACTION
 *
 *     User is interacting with the device, for example, touchscreen
 *     events are incoming.  CPU and GPU load may be expected soon,
 *     and it may be appropriate to raise speeds of CPU, memory bus,
 *     etc.  The data parameter is unused.
 *
 * POWER_HINT_LOW_POWER
 *
 *     Low power mode is activated or deactivated. Low power mode
 *     is intended to save battery at the cost of performance. The data
 *     parameter is non-zero when low power mode is activated, and zero
 *     when deactivated.
 *
 * POWER_HINT_CPU_BOOST
 *
 *     An operation is happening where it would be ideal for the CPU to
 *     be boosted for a specific duration. The data parameter is an
 *     integer value of the boost duration in microseconds.
 */
static void samsung_power_hint(struct power_module *module,
                                  power_hint_t hint,
                                  void *data)
{
    struct samsung_power_module *samsung_pwr = (struct samsung_power_module *) module;

    switch (hint) {
        case POWER_HINT_SET_PROFILE: {
            int32_t profile = *((intptr_t *)data);
            ALOGV("%s: POWER_HINT_SET_PROFILE", __func__);
            set_power_profile(samsung_pwr, profile);
            break;
        }
        case POWER_HINT_LOW_POWER: {
            ALOGV("%s: POWER_HINT_LOW_POWER", __func__);
            break;
        }
        case POWER_HINT_CPU_BOOST: {
            ALOGV("%s: POWER_HINT_CPU_BOOST", __func__);
            break;
        }
        case POWER_HINT_INTERACTION: {
            ALOGV("%s: POWER_HINT_INTERACTION", __func__);
            break;
        }
        default:
            break;
    }
}

static int samsung_get_feature(struct power_module *module __unused,
                               feature_t feature)
{
    if (feature == POWER_FEATURE_SUPPORTED_PROFILES) {
        return 3;
    }

    return -1;
}

static void samsung_set_feature(struct power_module *module, feature_t feature, int state __unused)
{
}

static void samsung_power_set_interactive(struct power_module *module, int on)
{
}

static struct hw_module_methods_t power_module_methods = {
    .open = NULL,
};

struct samsung_power_module HAL_MODULE_INFO_SYM = {
    .base = {
        .common = {
            .tag = HARDWARE_MODULE_TAG,
            .module_api_version = POWER_MODULE_API_VERSION_0_2,
            .hal_api_version = HARDWARE_HAL_API_VERSION,
            .id = POWER_HARDWARE_MODULE_ID,
            .name = "Samsung Power HAL",
            .author = "The CyanogenMod Project",
            .methods = &power_module_methods,
        },

        .init = samsung_power_init,
        .setInteractive = samsung_power_set_interactive,
        .powerHint = samsung_power_hint,
        .getFeature = samsung_get_feature,
        .setFeature = samsung_set_feature
    },
};
