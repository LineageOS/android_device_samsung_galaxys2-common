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

#ifndef TINYALSA_AUDIO_RIL_INTERFACE
#define TINYALSA_AUDIO_RIL_INTERFACE

#define AUDIO_RIL_INTERFACE_LIB	"libaudio-ril-interface.so"

struct audio_ril_interface {
	void *pdata;
	int (*mic_mute)(void *pdata, int mute);
	int (*voice_volume)(void *pdata, audio_devices_t device, float volume);
	int (*route)(void *pdata, audio_devices_t device);
};

#endif
