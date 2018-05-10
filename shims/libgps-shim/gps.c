/*
 * Copyright (C) 2016 The CyanogenMod Project <http://www.cyanogenmod.org>
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
#define LOG_TAG "libgps-shim"

#include <utils/Log.h>
#include <hardware/gps.h>

#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gps.h"
#define REAL_GPS_PATH "system/vendor/lib/hw/gps.exynos4.vendor.so"

const GpsInterface* (*vendor_get_gps_interface)(struct gps_device_t* dev);
const void* (*vendor_get_extension)(const char* name);
int (*vendor_init)(GpsCallbacks* gpsCallbacks);
void (*vendor_set_ref_location)(const AGpsRefLocation_vendor *agps_reflocation, size_t sz_struct);

GpsCallbacks *orgGpsCallbacks = NULL;
static GpsSvStatus gpsSvStatus_info;

static void log_location_vendor(char* func, GpsLocation* location) {
    ALOGD("%s: size=%3d flags=%x latitude=%f speed=%f bearing=%f accuracy=%f timestamp:%lu",
        func,
        location->size,
        location->flags,
        location->latitude,
        location->altitude,
        location->speed,
        location->bearing,
        location->accuracy,
        location->timestamp);
}

static void log_sv_info_vendor(char* func, GpsSvStatus_vendor* sv_info) {
    ALOGD("%s: size=%d num_svs=%d ephemeris_mask=%x almanac_mask=%x used_in_fix_mask=%x",
       func,
       sv_info->size,
       sv_info->num_svs,
       sv_info->ephemeris_mask,
       sv_info->almanac_mask,
       sv_info->used_in_fix_mask);
    for (int index = 0; index < sv_info->num_svs; index++) {
        ALOGD("%s:   svinfo[%d] - size=%3d prn=%3d snr=%3.1f elevation=%3.1f azimuth=%3.1f vendor=%3d",
            func,
            index,
            sv_info->sv_list[index].size,
            sv_info->sv_list[index].prn,
            sv_info->sv_list[index].snr,
            sv_info->sv_list[index].elevation,
            sv_info->sv_list[index].azimuth,
            sv_info->sv_list[index].vendor);
    }
}

static void log_gpsStatus_vendor(char* func, GpsStatus* status) {
    ALOGD("%s: size=%3d status=%3d", func, status->size, status->status);
}

static void copy_sv_info_from_vendor(GpsSvStatus_vendor source, GpsSvStatus* target) {
    target->size = source.size;
    target->num_svs = source.num_svs;
    target->ephemeris_mask = source.ephemeris_mask;
    target->almanac_mask = source.almanac_mask;
    target->used_in_fix_mask = source.used_in_fix_mask;
    for (int index = 0; index < source.num_svs; index++) {
	target->sv_list[index].size = source.sv_list[index].size;
        target->sv_list[index].prn = source.sv_list[index].prn;
        target->sv_list[index].snr = source.sv_list[index].snr;
        target->sv_list[index].elevation = source.sv_list[index].elevation;
        target->sv_list[index].azimuth = source.sv_list[index].azimuth;
    }
}

static void shim_location_cb(GpsLocation* location) {
    log_location_vendor(__func__, location);
    orgGpsCallbacks->location_cb(location);
}

static void shim_status_cb(GpsStatus* status) {
    log_gpsStatus_vendor(__func__, status);
    orgGpsCallbacks->sv_status_cb(status);
}

static void shim_sv_status_cb(GpsSvStatus* sv_info) {
    GpsSvStatus_vendor* gpsSvStatus_vendor_info = (GpsSvStatus_vendor*)sv_info;
    log_sv_info_vendor(__func__, gpsSvStatus_vendor_info);
    copy_sv_info_from_vendor(*gpsSvStatus_vendor_info, &gpsSvStatus_info);
    orgGpsCallbacks->sv_status_cb(&gpsSvStatus_info);
}

static void shim_nmea_cb(GpsUtcTime timestamp, const char* nmea, int length) {
    orgGpsCallbacks->nmea_cb(timestamp, nmea, length);
}

static void shim_set_capabilities_cb(uint32_t capabilities) {
    ALOGD("%s: capabilities=%d", __func__, capabilities);
    orgGpsCallbacks->set_capabilities_cb(capabilities);
}

static void shim_acquire_wakelock_cb() {
    orgGpsCallbacks->acquire_wakelock_cb();
}

static void shim_release_wakelock_cb() {
    orgGpsCallbacks->release_wakelock_cb();
}

static pthread_t shim_create_thread_cb(const char* name, void (*start)(void *), void* arg) {
    return orgGpsCallbacks->create_thread_cb(name, start, arg);
}

static void shim_request_utc_time_cb() {
    orgGpsCallbacks->request_utc_time_cb();
}

void shim_set_ref_location(AGpsRefLocation *agps_reflocation, size_t sz_struct) {
	AGpsRefLocation_vendor vendor_ref;
	if (sizeof(AGpsRefLocation_vendor) > sz_struct) {
		ALOGE("%s: AGpsRefLocation is too small, bailing out!", __func__);
		return;
	}
	ALOGD("%s: shimming AGpsRefLocation", __func__);
	ALOGD("%s: AGpsRefLocation (%d) | AGpsRefLocation_vendor(%d)", __func__, sizeof(AGpsRefLocation), sizeof(AGpsRefLocation_vendor));
	vendor_ref.type = agps_reflocation->type;
	vendor_ref.u.cellID.type = agps_reflocation->u.cellID.type;
	vendor_ref.u.cellID.mcc = agps_reflocation->u.cellID.mcc;
	vendor_ref.u.cellID.mnc = agps_reflocation->u.cellID.mnc;
	vendor_ref.u.cellID.lac = agps_reflocation->u.cellID.lac;
	vendor_ref.u.cellID.psc = 65535;
	vendor_ref.u.cellID.cid = agps_reflocation->u.cellID.cid;
	vendor_ref.u.mac = agps_reflocation->u.mac;
	ALOGD("%s: Executing vendor_set_ref_location= > type:%d mcc:%d mnc:%d lac:%d psc:%d cid:%d mac:%d",
		__func__,
		vendor_ref.u.cellID.type,
		vendor_ref.u.cellID.mcc,
		vendor_ref.u.cellID.mnc,
		vendor_ref.u.cellID.lac,
		vendor_ref.u.cellID.psc,
		vendor_ref.u.cellID.cid,
		vendor_ref.u.mac);
	vendor_set_ref_location(&vendor_ref, sizeof(AGpsRefLocation_vendor));

	agps_reflocation->type = vendor_ref.type;
	agps_reflocation->u.cellID.type = vendor_ref.u.cellID.type;
	agps_reflocation->u.cellID.mcc = vendor_ref.u.cellID.mcc;
	agps_reflocation->u.cellID.mnc = vendor_ref.u.cellID.mnc;
	agps_reflocation->u.cellID.lac = vendor_ref.u.cellID.lac;
	agps_reflocation->u.cellID.cid = vendor_ref.u.cellID.cid;
	agps_reflocation->u.mac = vendor_ref.u.mac;
}

const void* shim_get_extension(const char* name) {
	ALOGD("%s(%s)", __func__, name);
	if (strcmp(name, AGPS_RIL_INTERFACE) == 0) {
		// RIL interface
		AGpsRilInterface *ril = (AGpsRilInterface*)vendor_get_extension(name);
		// now we shim the ref_location callback
		ALOGD("%s: shimming RIL ref_location callback", __func__);
		vendor_set_ref_location = ril->set_ref_location;
		ril->set_ref_location = shim_set_ref_location;
		return ril;
	} else {
		return vendor_get_extension(name);
	}
}

int shim_init (GpsCallbacks* gpsCallbacks) {
	ALOGD("%s: shimming GpsCallbacks", __func__);
        orgGpsCallbacks = gpsCallbacks;
        GpsCallbacks_vendor vendor_gpsCallbacks;
	vendor_gpsCallbacks.size = sizeof(GpsCallbacks_vendor);
	vendor_gpsCallbacks.location_cb = shim_location_cb;
	vendor_gpsCallbacks.status_cb = shim_status_cb;
	vendor_gpsCallbacks.sv_status_cb = shim_sv_status_cb;
	vendor_gpsCallbacks.nmea_cb = shim_nmea_cb;
	vendor_gpsCallbacks.set_capabilities_cb = shim_set_capabilities_cb;
	vendor_gpsCallbacks.acquire_wakelock_cb = shim_acquire_wakelock_cb;
	vendor_gpsCallbacks.release_wakelock_cb = shim_release_wakelock_cb;
	vendor_gpsCallbacks.create_thread_cb = shim_create_thread_cb;
	vendor_gpsCallbacks.request_utc_time_cb = shim_request_utc_time_cb;

	ALOGD("%s: Calling vendor init", __func__);
	return vendor_init(&vendor_gpsCallbacks);
}

const GpsInterface* shim_get_gps_interface(struct gps_device_t* dev) {
	GpsInterface *halInterface = vendor_get_gps_interface(dev);

	ALOGD("%s: shimming vendor get_extension", __func__);
	vendor_get_extension = halInterface->get_extension;
	halInterface->get_extension = &shim_get_extension;

	ALOGD("%s: shimming vendor init", __func__);
	vendor_init = halInterface->init;
	halInterface->init = &shim_init;

	return halInterface;
}

static int open_gps(const struct hw_module_t* module, char const* name,
		struct hw_device_t** device) {
	void *realGpsLib;
	int gpsHalResult;
	struct hw_module_t *realHalSym;

	struct gps_device_t **gps = (struct gps_device_t **)device;

	realGpsLib = dlopen(REAL_GPS_PATH, RTLD_LOCAL);
	if (!realGpsLib) {
		ALOGE("Failed to load real GPS HAL '" REAL_GPS_PATH "': %s", dlerror());
		return -EINVAL;
	}

	realHalSym = (struct hw_module_t*)dlsym(realGpsLib, HAL_MODULE_INFO_SYM_AS_STR);
	if (!realHalSym) {
		ALOGE("Failed to locate the GPS HAL module sym: '" HAL_MODULE_INFO_SYM_AS_STR "': %s", dlerror());
		goto dl_err;
	}

	int result = realHalSym->methods->open(realHalSym, name, device);
	if (result < 0) {
		ALOGE("Real GPS open method failed: %d", result);
		goto dl_err;
	}
	ALOGD("Successfully loaded real GPS hal, shimming get_gps_interface...");
	// now, we shim hw_device_t
	vendor_get_gps_interface = (*gps)->get_gps_interface;
	(*gps)->get_gps_interface = &shim_get_gps_interface;

	return 0;
dl_err:
	dlclose(realGpsLib);
	return -EINVAL;
}

static struct hw_module_methods_t gps_module_methods = {
	.open = open_gps
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
	.tag = HARDWARE_MODULE_TAG,
	.module_api_version = 1,
	.hal_api_version = 0,
	.id = GPS_HARDWARE_MODULE_ID,
	.name = "GSD4t GPS shim",
	.author = "The CyanogenMod Project",
	.methods = &gps_module_methods
};

