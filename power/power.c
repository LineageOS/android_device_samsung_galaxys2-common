/*
 * Copyright (C) 2012 The Android Open Source Project
 * Copyright (C) 2014 The CyanogenMod Project
 * Copyright (C) 2014-2015 Andreas Schneider <asn@cryptomilk.org>
 * Copyright (C) 2014-2015 Christopher N. Hesse <raymanfx@gmail.com>
 * Copyright (C) 2017 Dominggoes Isakh <drjisakh@gmail.com>
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
#define LOG_TAG "PegasusPowerHAL"

#include <hardware/hardware.h>
#include <hardware/power.h>

#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// #define LOG_NDEBUG 0
#define DEBUG 0
#include <log/log.h>

#include "power.h"


#define PEGASUSQ_PATH "/sys/devices/system/cpu/cpufreq/pegasusq/"
#define MINMAX_CPU_PATH "/sys/power/"

#define US_TO_NS (1000L)

static int current_power_profile = -1;
static bool is_low_power = false;
static bool is_vsync_active = false;

/**********************************************************
 *** HELPER FUNCTIONS
 **********************************************************/
/*
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
*/

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

static int sysfs_write_str(char *path, char *s) {
    char buf[80];
    int len;
    int ret = 0;
    int fd;

    fd = open(path, O_WRONLY);
    if (fd < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error opening %s: %s\n", path, buf);
        return -1 ;
    }

    len = write(fd, s, strlen(s));
    if (len < 0) {
        strerror_r(errno, buf, sizeof(buf));
        ALOGE("Error writing to %s: %s\n", path, buf);
        ret = -1;
    }

    close(fd);

    return ret;
}

static int sysfs_write_int(char *path, int value) {
    char buf[80];
    snprintf(buf, 80, "%d", value);
    return sysfs_write_str(path, buf);
}

static int sysfs_write_long(char *path, long value) {
    char buf[80];
    snprintf(buf, 80, "%ld", value);
    return sysfs_write_str(path, buf);
}

#ifdef LOG_NDEBUG
#define WRITE_PEGASUSQ_PARAM(profile, param) do { \
    ALOGV("%s: WRITE_PEGASUSQ_PARAM(profile=%d, param=%s): new val => %d", __func__, profile, #param, profiles[profile].param); \
    sysfs_write_int(PEGASUSQ_PATH #param, profiles[profile].param); \
} while (0)
#define WRITE_LOW_POWER_PARAM(profile, param) do { \
    ALOGV("%s: WRITE_LOW_POWER_PARAM(profile=%d, param=%s): new val => %d", \
            __func__, profile, #param, profiles_low_power[profile].param); \
    sysfs_write_int(PEGASUSQ_PATH #param, profiles_low_power[profile].param); \
} while (0)
#define WRITE_PEGASUSQ_VALUE(param, value) do { \
    ALOGV("%s: WRITE_PEGASUSQ_VALUE(param=%s, value=%d)", __func__, #param, value); \
    sysfs_write_int(PEGASUSQ_PATH #param, value); \
} while (0)
#define WRITE_MINMAX_CPU(param, value) do { \
    ALOGV("%s: WRITE_MINMAX_CPU(param=%s, value=%d)", __func__, #param, value); \
    sysfs_write_int(MINMAX_CPU_PATH #param, value); \
} while(0)
#else
#define WRITE_PEGASUSQ_PARAM(profile, param) sysfs_write_int(PEGASUSQ_PATH #param, profiles[profile].param)
#define WRITE_LOW_POWER_PARAM(profile, param) sysfs_write_int(PEGASUSQ_PATH #param, profiles_low_power[profile].param)
#define WRITE_PEGASUSQ_VALUE(param, value)   sysfs_write_int(PEGASUSQ_PATH #param, value)
#define WRITE_MINMAX_CPU(param, value) sysfs_write_int(MINMAX_CPU_PATH #param, value)
#endif
static bool check_governor_pegasusq() {
    struct stat s;
    int err = stat(PEGASUSQ_PATH, &s);
    if (err != 0) return false;
    return S_ISDIR(s.st_mode);
}

static bool is_profile_valid(int profile) {
    return profile >= 0 && profile < PROFILE_MAX;
}

static void set_power_profile(int profile) {
    if (!is_profile_valid(profile)) {
        ALOGE("%s: unknown profile: %d", __func__, profile);
        return;
    }

    if (profile == current_power_profile) return;

    if (check_governor_pegasusq()) {
        WRITE_PEGASUSQ_PARAM(profile, hotplug_freq_1_1);
        WRITE_PEGASUSQ_PARAM(profile, hotplug_freq_2_0);
        WRITE_PEGASUSQ_PARAM(profile, hotplug_rq_1_1);
        WRITE_PEGASUSQ_PARAM(profile, hotplug_rq_2_0);
        WRITE_MINMAX_CPU(cpufreq_max_limit, profiles[profile].max_freq);
        WRITE_MINMAX_CPU(cpufreq_min_limit, profiles[profile].min_freq);
        WRITE_PEGASUSQ_PARAM(profile, freq_step);
        WRITE_PEGASUSQ_PARAM(profile, up_threshold);
        WRITE_PEGASUSQ_PARAM(profile, up_threshold_at_min_freq);
        WRITE_PEGASUSQ_PARAM(profile, freq_for_responsiveness);
        WRITE_PEGASUSQ_PARAM(profile, down_differential);
        WRITE_PEGASUSQ_PARAM(profile, min_cpu_lock);
        WRITE_PEGASUSQ_PARAM(profile, max_cpu_lock);
        WRITE_PEGASUSQ_PARAM(profile, cpu_up_rate);
        WRITE_PEGASUSQ_PARAM(profile, cpu_down_rate);
        WRITE_PEGASUSQ_PARAM(profile, sampling_rate);
        WRITE_PEGASUSQ_PARAM(profile, io_is_busy);
        WRITE_PEGASUSQ_PARAM(profile, boost_freq);
        WRITE_PEGASUSQ_PARAM(profile, boost_mincpus);
    } else {
        switch (profile) {
            case PROFILE_POWER_SAVE:
                sysfs_write(GOVERNOR_PATH, GOV_POWERSAVE);
                ALOGD("%s: activate powersave governor", __func__);
                break;
            case PROFILE_BALANCED:
                sysfs_write(GOVERNOR_PATH, GOV_ONDEMAND);
                ALOGD("%s: activate ondemand governor", __func__);
                break;
            case PROFILE_PERFORMANCE:
                sysfs_write(GOVERNOR_PATH, GOV_PERFORMANCE);
                ALOGD("%s: activate performance governor", __func__);
                break;
        }
    }
    current_power_profile = profile;

    if (DEBUG) ALOGV("%s: %d", __func__, profile);
}

static void boost(long boost_time) {
#ifdef USE_PEGASUSQ_BOOSTING
    if (is_vsync_active || !check_governor_pegasusq()) return;
    if (boost_time == -1) {
        sysfs_write_int(PEGASUSQ_PATH "boost_lock_time", -1);
#ifdef LOG_NDEBUG
        ALOGV("%s: (param=boost_time, value=%d)", __func__, -1);
#endif
    } else {
        sysfs_write_long(PEGASUSQ_PATH "boost_lock_time", boost_time);
#ifdef LOG_NDEBUG
        ALOGV("%s: (param=boost_time, value=%ld)", __func__, boost_time);
#endif
    }
#endif
}

static void end_boost() {
#ifdef USE_PEGASUSQ_BOOSTING
    if (is_vsync_active || !check_governor_pegasusq()) return;
    sysfs_write_int(PEGASUSQ_PATH "boost_lock_time", 0);
#ifdef LOG_NDEBUG
    ALOGV("%s: (param=boost_time, value=%d)", __func__, 0);
#endif
#endif
}

static void set_power(bool low_power) {
    if (!is_profile_valid(current_power_profile)) {
        if (DEBUG) ALOGV("%s: current_power_profile not set yet", __func__);
        return;
    }

    if (!check_governor_pegasusq()) return;

    if (is_low_power == low_power) return;

    if (low_power) {
        end_boost();
        WRITE_LOW_POWER_PARAM(current_power_profile, hotplug_freq_1_1);
        WRITE_LOW_POWER_PARAM(current_power_profile, hotplug_freq_2_0);
        WRITE_LOW_POWER_PARAM(current_power_profile, hotplug_rq_1_1);
        WRITE_LOW_POWER_PARAM(current_power_profile, hotplug_rq_2_0);
        WRITE_MINMAX_CPU(cpufreq_max_limit, profiles_low_power[current_power_profile].max_freq);
        WRITE_MINMAX_CPU(cpufreq_min_limit, profiles_low_power[current_power_profile].min_freq);
        WRITE_LOW_POWER_PARAM(current_power_profile, freq_step);
        WRITE_LOW_POWER_PARAM(current_power_profile, up_threshold);
        WRITE_LOW_POWER_PARAM(current_power_profile, up_threshold_at_min_freq);
        WRITE_LOW_POWER_PARAM(current_power_profile, freq_for_responsiveness);
        WRITE_LOW_POWER_PARAM(current_power_profile, down_differential);
        WRITE_LOW_POWER_PARAM(current_power_profile, min_cpu_lock);
        WRITE_LOW_POWER_PARAM(current_power_profile, max_cpu_lock);
        WRITE_LOW_POWER_PARAM(current_power_profile, cpu_down_rate);
        WRITE_LOW_POWER_PARAM(current_power_profile, sampling_rate);
        WRITE_LOW_POWER_PARAM(current_power_profile, io_is_busy);
        is_low_power = true;
    } else {
        WRITE_PEGASUSQ_PARAM(current_power_profile, hotplug_freq_1_1);
        WRITE_PEGASUSQ_PARAM(current_power_profile, hotplug_freq_2_0);
        WRITE_PEGASUSQ_PARAM(current_power_profile, hotplug_rq_1_1);
        WRITE_PEGASUSQ_PARAM(current_power_profile, hotplug_rq_2_0);
        WRITE_MINMAX_CPU(cpufreq_max_limit, profiles[current_power_profile].max_freq);
        WRITE_MINMAX_CPU(cpufreq_min_limit, profiles[current_power_profile].min_freq);
        WRITE_PEGASUSQ_PARAM(current_power_profile, freq_step);
        WRITE_PEGASUSQ_PARAM(current_power_profile, up_threshold);
        WRITE_PEGASUSQ_PARAM(current_power_profile, up_threshold_at_min_freq);
        WRITE_PEGASUSQ_PARAM(current_power_profile, freq_for_responsiveness);
        WRITE_PEGASUSQ_PARAM(current_power_profile, down_differential);
        WRITE_PEGASUSQ_PARAM(current_power_profile, min_cpu_lock);
        WRITE_PEGASUSQ_PARAM(current_power_profile, max_cpu_lock);
        WRITE_PEGASUSQ_PARAM(current_power_profile, cpu_down_rate);
        WRITE_PEGASUSQ_PARAM(current_power_profile, sampling_rate);
        WRITE_PEGASUSQ_PARAM(current_power_profile, io_is_busy);
        WRITE_PEGASUSQ_PARAM(current_power_profile, boost_freq);
        WRITE_PEGASUSQ_PARAM(current_power_profile, boost_mincpus);
        is_low_power = false;
    }
}



/*
 * (*init)() performs power management setup actions at runtime
 * startup, such as to set default cpufreq parameters.  This is
 * called only by the Power HAL instance loaded by
 * PowerManagerService.
 */
void power_init(void) {
    set_power_profile(PROFILE_BALANCED);
    if (DEBUG) ALOGV("%s", __func__);
}

/*
 * The setInteractive function performs power management actions upon the
 * system entering interactive state (that is, the system is awake and ready
 * for interaction, often with UI devices such as display and touchscreen
 * enabled) or non-interactive state (the system appears asleep, display
 * usually turned off).  The non-interactive state is usually entered after a
 * period of inactivity, in order to conserve battery power during such
 * inactive periods.
 *
 * Typical actions are to turn on or off devices and adjust cpufreq parameters.
 * This function may also call the appropriate interfaces to allow the kernel
 * to suspend the system to low-power sleep state when entering non-interactive
 * state, and to disallow low-power suspend when the system is in interactive
 * state.  When low-power suspend state is allowed, the kernel may suspend the
 * system whenever no wakelocks are held.
 *
 * on is non-zero when the system is transitioning to an interactive / awake
 * state, and zero when transitioning to a non-interactive / asleep state.
 *
 * This function is called to enter non-interactive state after turning off the
 * screen (if present), and called to enter interactive state prior to turning
 * on the screen.
 */
void power_set_interactive(int on) {
    if (!is_profile_valid(current_power_profile)) {
        ALOGD("%s: no power profile selected", __func__);
        return;
    }

    if (!check_governor_pegasusq()) return;
    if (DEBUG) ALOGV("%s: setting interactive => %d", __func__, on);
    set_power(!on);
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
void power_hint(power_hint_t hint, void *data) {
    int32_t val;

    if (hint == POWER_HINT_SET_PROFILE) {
        if (DEBUG) ALOGV("%s: set profile %d", __func__, *(int32_t *)data);
        if (is_vsync_active) {
            is_vsync_active = false;
            end_boost();
        }
        set_power_profile(*(int32_t *)data);

    }

    if (current_power_profile == PROFILE_POWER_SAVE) return;

    switch (hint) {
        case POWER_HINT_INTERACTION:
            if (data) {
                if (DEBUG) ALOGV("%s: interaction", __func__);
                val = *(int32_t *)data;
                if (val > 0) {
                    boost(val * US_TO_NS);
                } else {
                    boost(profiles[current_power_profile].interaction_boost_time);
                }
            }
            break;
        case POWER_HINT_LAUNCH:
            if (DEBUG) ALOGV("%s: launch", __func__);
            boost(profiles[current_power_profile].launch_boost_time);
            break;
        case POWER_HINT_CPU_BOOST:
            if (DEBUG) ALOGV("%s: cpu_boost", __func__);
            boost((*(int32_t *)data) * US_TO_NS);
            break;
        default:
            break;
    }
}

int get_number_of_profiles()
{
    return PROFILE_MAX;
}
