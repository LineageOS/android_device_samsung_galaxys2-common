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

#include "include/audio_ril_interface.h"
#include "audio_hw.h"

#ifndef TINYALSA_AUDIO_RIL_INTERFACE_H
#define TINYALSA_AUDIO_RIL_INTERFACE_H

struct tinyalsa_audio_ril_interface {
	struct audio_ril_interface *interface;
	struct tinyalsa_audio_device *device;

	void *dl_handle;

	audio_devices_t device_current;

	pthread_mutex_t lock;
};

int audio_ril_interface_set_mic_mute(struct tinyalsa_audio_ril_interface *ril_interface, bool state);
int audio_ril_interface_set_voice_volume(struct tinyalsa_audio_ril_interface *ril_interface, audio_devices_t device, float volume);
int audio_ril_interface_set_route(struct tinyalsa_audio_ril_interface *ril_interface, audio_devices_t device);

void audio_ril_interface_close(struct audio_hw_device *dev,
	struct tinyalsa_audio_ril_interface *interface);
int audio_ril_interface_open(struct audio_hw_device *dev, audio_devices_t device,
	struct tinyalsa_audio_ril_interface **ril_interface);

#endif
