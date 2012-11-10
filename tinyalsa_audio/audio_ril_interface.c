/*
 * Copyright (C) 2012 Paul Kocialkowski <contact@paulk.fr>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define LOG_TAG "TinyALSA-Audio RIL Interface"

#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <dlfcn.h>
#include <sys/time.h>

#include <cutils/log.h>

#define EFFECT_UUID_NULL EFFECT_UUID_NULL_RIL
#define EFFECT_UUID_NULL_STR EFFECT_UUID_NULL_STR_RIL
#include "audio_hw.h"

#include "include/audio_ril_interface.h"
#include "audio_ril_interface.h"

int audio_ril_interface_set_mic_mute(struct tinyalsa_audio_ril_interface *ril_interface, bool state)
{
	int rc;

	if(ril_interface == NULL)
		return -1;

	ALOGD("%s(%d)", __func__, state);

	pthread_mutex_lock(&ril_interface->lock);

	if(ril_interface->interface->mic_mute == NULL)
		goto error;

	rc = ril_interface->interface->mic_mute(ril_interface->interface->pdata, (int) state);
	if(rc < 0) {
		ALOGE("Failed to set RIL interface mic mute");
		goto error;
	}

	pthread_mutex_unlock(&ril_interface->lock);

	return 0;

error:
	pthread_mutex_unlock(&ril_interface->lock);

	return -1;
}

int audio_ril_interface_set_voice_volume(struct tinyalsa_audio_ril_interface *ril_interface,
	audio_devices_t device, float volume)
{
	int rc;

	if(ril_interface == NULL)
		return -1;

	ALOGD("%s(%d, %f)", __func__, device, volume);

	pthread_mutex_lock(&ril_interface->lock);

	if(ril_interface->interface->voice_volume == NULL)
		goto error;

	rc = ril_interface->interface->voice_volume(ril_interface->interface->pdata, device, volume);
	if(rc < 0) {
		ALOGE("Failed to set RIL interface voice volume");
		goto error;
	}

	pthread_mutex_unlock(&ril_interface->lock);

	return 0;

error:
	pthread_mutex_unlock(&ril_interface->lock);

	return -1;
}

int audio_ril_interface_set_route(struct tinyalsa_audio_ril_interface *ril_interface, audio_devices_t device)
{
	int rc;

	ALOGD("%s(%d)", __func__, device);

	if(ril_interface == NULL)
		return -1;

	pthread_mutex_lock(&ril_interface->lock);

	ril_interface->device_current = device;

	if(ril_interface->interface->route == NULL)
		goto error;

	rc = ril_interface->interface->route(ril_interface->interface->pdata, device);
	if(rc < 0) {
		ALOGE("Failed to set RIL interface route");
		goto error;
	}

	pthread_mutex_unlock(&ril_interface->lock);

	return 0;

error:
	pthread_mutex_unlock(&ril_interface->lock);

	return -1;
}

/*
 * Interface
 */

void audio_ril_interface_close(struct audio_hw_device *dev,
	struct tinyalsa_audio_ril_interface *ril_interface)
{
	void (*audio_ril_interface_close)(struct audio_ril_interface *interface);

	struct tinyalsa_audio_device *tinyalsa_audio_device;

	ALOGD("%s(%p)", __func__, ril_interface);

	if(ril_interface->dl_handle != NULL) {
		audio_ril_interface_close = (void (*)(struct audio_ril_interface *interface))
			dlsym(ril_interface->dl_handle, "audio_ril_interface_close");

		if(audio_ril_interface_close == NULL) {
			ALOGE("Unable to get close function from: %s", AUDIO_RIL_INTERFACE_LIB);
		} else {
			audio_ril_interface_close(ril_interface->interface);
		}

		dlclose(ril_interface->dl_handle);
		ril_interface->dl_handle = NULL;
	}

	if(ril_interface != NULL)
		free(ril_interface);

	if(dev == NULL)
		return;

	tinyalsa_audio_device = (struct tinyalsa_audio_device *) dev;

	tinyalsa_audio_device->ril_interface = NULL;
}

int audio_ril_interface_open(struct audio_hw_device *dev, audio_devices_t device,
	struct tinyalsa_audio_ril_interface **ril_interface)
{
	struct audio_ril_interface *(*audio_ril_interface_open)(void);

	struct tinyalsa_audio_device *tinyalsa_audio_device;
	struct tinyalsa_audio_ril_interface *tinyalsa_audio_ril_interface;
	struct audio_ril_interface *interface;
	void *dl_handle;
	int rc;

	ALOGD("%s(%p, %d, %p)", __func__, dev, device, ril_interface);

	if(dev == NULL || ril_interface == NULL)
		return -EINVAL;

	tinyalsa_audio_device = (struct tinyalsa_audio_device *) dev;
	tinyalsa_audio_ril_interface = calloc(1, sizeof(struct tinyalsa_audio_ril_interface));

	if(tinyalsa_audio_ril_interface == NULL)
		return -ENOMEM;

	tinyalsa_audio_ril_interface->device = tinyalsa_audio_device;
	tinyalsa_audio_device->ril_interface = tinyalsa_audio_ril_interface;

	dl_handle = dlopen(AUDIO_RIL_INTERFACE_LIB, RTLD_NOW);
	if(dl_handle == NULL) {
		ALOGE("Unable to dlopen lib: %s", AUDIO_RIL_INTERFACE_LIB);
		goto error_interface;
	}

	audio_ril_interface_open = (struct audio_ril_interface *(*)(void))
		dlsym(dl_handle, "audio_ril_interface_open");
	if(audio_ril_interface_open == NULL) {
		ALOGE("Unable to get open function from: %s", AUDIO_RIL_INTERFACE_LIB);
		goto error_interface;
	}

	interface = audio_ril_interface_open();
	if(interface == NULL) {
		ALOGE("Unable to open audio ril interface");
		goto error_interface;
	}

	tinyalsa_audio_ril_interface->interface = interface;
	tinyalsa_audio_ril_interface->dl_handle = dl_handle;

	if(device) {
		tinyalsa_audio_ril_interface->device_current = device;
		audio_ril_interface_set_route(tinyalsa_audio_ril_interface, device);
	}

	*ril_interface = tinyalsa_audio_ril_interface;

	return 0;

error_interface:
	*ril_interface = NULL;
	free(tinyalsa_audio_ril_interface);
	tinyalsa_audio_device->ril_interface = NULL;

	if(dl_handle != NULL)
		dlclose(dl_handle);

	return -1;
}
