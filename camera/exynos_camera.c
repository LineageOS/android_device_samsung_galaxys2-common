/*
 * Copyright (C) 2013 Paul Kocialkowski
 *
 * Based on crespo libcamera and exynos4 hal libcamera:
 * Copyright 2008, The Android Open Source Project
 * Copyright 2010, Samsung Electronics Co. LTD
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

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <jpeg_api.h>

#define LOG_TAG "exynos_camera"
#include <utils/Log.h>
#include <utils/Timers.h>

#include "exynos_camera.h"

/*
 * Devices configurations
 */

struct exynos_camera_preset exynos_camera_presets_galaxys2[] = {
	{
		.name = "M5MO",
		.facing = CAMERA_FACING_BACK,
		.orientation = 90,
		.rotation = 0,
		.hflip = 0,
		.vflip = 0,
		.picture_format = V4L2_PIX_FMT_JPEG,
		.focal_length = 4.03f,
		.horizontal_view_angle = 60.5f,
		.vertical_view_angle = 47.1f,
		.metering = METERING_CENTER,
		.params = {
			.preview_size_values = "1280x720,640x480,720x480,800x480,800x450,352x288,320x240,176x144",
			.preview_size = "640x480",
			.preview_format_values = "yuv420sp,yuv420p,rgb565",
			.preview_format = "yuv420sp",
			.preview_frame_rate_values = "30,25,20,15,10,7",
			.preview_frame_rate = 30,
			.preview_fps_range_values = "(7000,30000)",
			.preview_fps_range = "7000,30000",

			.picture_size_values = "3264x2448,3264x1968,2048x1536,2048x1232,1280x960,800x480,640x480",
			.picture_size = "3264x2448",
			.picture_format_values = "jpeg",
			.picture_format = "jpeg",
			.jpeg_thumbnail_size_values = "320x240,400x240,0x0",
			.jpeg_thumbnail_width = 320,
			.jpeg_thumbnail_height = 240,
			.jpeg_thumbnail_quality = 100,
			.jpeg_quality = 90,

			.recording_size = "720x480",
			.recording_size_values = "1920x1080,1280x720,720x480,640x480",
			.recording_format = "yuv420sp",

			.focus_mode = "auto",
			.focus_mode_values = "auto,infinity,macro,fixed,facedetect,continuous-video",
			.focus_distances = "0.15,1.20,Infinity",
			.focus_areas = "(0,0,0,0,0)",
			.max_num_focus_areas = 1,

			.zoom_supported = 1,
			.smooth_zoom_supported = 0,
			.zoom_ratios = "100,102,104,109,111,113,119,121,124,131,134,138,146,150,155,159,165,170,182,189,200,213,222,232,243,255,283,300,319,364,400",
			.zoom = 0,
			.max_zoom = 30,

			.flash_mode = "off",
			.flash_mode_values = "off,auto,on,torch",

			.exposure_compensation = 0,
			.exposure_compensation_step = 0.5,
			.min_exposure_compensation = -4,
			.max_exposure_compensation = 4,

			.whitebalance = "auto",
			.whitebalance_values = "auto,incandescent,fluorescent,daylight,cloudy-daylight",

			.scene_mode = "auto",
			.scene_mode_values = "auto,portrait,landscape,night,beach,snow,sunset,fireworks,sports,party,candlelight,dusk-dawn,fall-color,back-light,text",

			.effect = "none",
			.effect_values = "none,mono,negative,sepia,aqua",

			.iso = "auto",
			.iso_values = "auto,ISO50,ISO100,ISO200,ISO400,ISO800",
		},
	},
	{
		.name = "S5K5BAFX",
		.facing = CAMERA_FACING_FRONT,
		.orientation = 270,
		.rotation = 0,
		.hflip = 0,
		.vflip = 0,
		.picture_format = V4L2_PIX_FMT_YUYV,
		.focal_length = 2.73f,
		.horizontal_view_angle = 51.2f,
		.vertical_view_angle = 39.4f,
		.metering = METERING_CENTER,
		.params = {
			.preview_size_values = "640x480,352x288,320x240,176x144",
			.preview_size = "640x480",
			.preview_format_values = "yuv420sp,yuv420p,rgb565",
			.preview_format = "yuv420sp",
			.preview_frame_rate_values = "30,25,20,15,10,7",
			.preview_frame_rate = 30,
			.preview_fps_range_values = "(7000,30000)",
			.preview_fps_range = "7000,30000",

			.picture_size_values = "1600x1200,640x480",
			.picture_size = "1600x1200",
			.picture_format_values = "jpeg",
			.picture_format = "jpeg",
			.jpeg_thumbnail_size_values = "160x120,0x0",
			.jpeg_thumbnail_width = 160,
			.jpeg_thumbnail_height = 120,
			.jpeg_thumbnail_quality = 100,
			.jpeg_quality = 90,

			.recording_size = "640x480",
			.recording_size_values = "640x480",
			.recording_format = "yuv420sp",

			.focus_mode = "fixed",
			.focus_mode_values = "fixed",
			.focus_distances = "0.20,0.25,Infinity",
			.focus_areas = NULL,
			.max_num_focus_areas = 0,

			.zoom_supported = 0,

			.flash_mode = NULL,
			.flash_mode_values = NULL,

			.exposure_compensation = 0,
			.exposure_compensation_step = 0.5,
			.min_exposure_compensation = -4,
			.max_exposure_compensation = 4,

			.whitebalance = NULL,
			.whitebalance_values = NULL,

			.scene_mode = NULL,
			.scene_mode_values = NULL,

			.effect = NULL,
			.effect_values = NULL,

			.iso = "auto",
			.iso_values = "auto",
		},
	},
};

struct exynos_v4l2_node exynos_v4l2_nodes_galaxys2[] = {
	{
		.id = 0,
		.node = "/dev/video0",
	},
	{
		.id = 1,
		.node = "/dev/video1",
	},
	{
		.id = 2,
		.node = "/dev/video2",
	},
};

struct exynox_camera_config exynos_camera_config_galaxys2 = {
	.presets = (struct exynos_camera_preset *) &exynos_camera_presets_galaxys2,
	.presets_count = 2,
	.v4l2_nodes = (struct exynos_v4l2_node *) &exynos_v4l2_nodes_galaxys2,
	.v4l2_nodes_count = 3,
};

/*
 * Exynos Camera
 */

struct exynox_camera_config *exynos_camera_config =
	&exynos_camera_config_galaxys2;

int exynos_camera_init(struct exynos_camera *exynos_camera, int id)
{
	char firmware_version[7] = { 0 };
	struct exynos_v4l2_ext_control control;
	int rc;

	if (exynos_camera == NULL || id >= exynos_camera->config->presets_count)
		return -EINVAL;

	// Init FIMC1
	rc = exynos_v4l2_open(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("Unable to open v4l2 device");
		return -1;
	}

	rc = exynos_v4l2_querycap_cap(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: querycap failed", __func__);
		return -1;
	}

	rc = exynos_v4l2_enum_input(exynos_camera, 0, id);
	if (rc < 0) {
		ALOGE("%s: enum input failed", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_input(exynos_camera, 0, id);
	if (rc < 0) {
		ALOGE("%s: s input failed", __func__);
		return -1;
	}

	// Init FIMC2
	rc = exynos_v4l2_open(exynos_camera, 2);
	if (rc < 0) {
		ALOGE("Unable to open v4l2 device");
		return -1;
	}

	rc = exynos_v4l2_querycap_cap(exynos_camera, 2);
	if (rc < 0) {
		ALOGE("%s: querycap failed", __func__);
		return -1;
	}

	rc = exynos_v4l2_enum_input(exynos_camera, 2, id);
	if (rc < 0) {
		ALOGE("%s: enum input failed", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_input(exynos_camera, 2, id);
	if (rc < 0) {
		ALOGE("%s: s input failed", __func__);
		return -1;
	}

	// Get firmware information
	memset(&control, 0, sizeof(control));
	control.id = V4L2_CID_CAM_SENSOR_FW_VER;
	control.data.string = firmware_version;

	rc = exynos_v4l2_g_ext_ctrls(exynos_camera, 0, (struct v4l2_ext_control *) &control, 1);
	if (rc < 0) {
		ALOGE("%s: g ext ctrls failed", __func__);
	} else {
		ALOGD("Firmware version: %s", firmware_version);
	}

	// Params
	rc = exynos_camera_params_init(exynos_camera, id);
	if (rc < 0)
		ALOGE("%s: Unable to init params", __func__);

	// Gralloc
	rc = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, (const struct hw_module_t **) &exynos_camera->gralloc);
	if (rc)
		ALOGE("%s: Unable to get gralloc module", __func__);

	return 0;
}

void exynos_camera_deinit(struct exynos_camera *exynos_camera)
{
	int i;
	int id;

	if (exynos_camera == NULL || exynos_camera->config == NULL)
		return;

	exynos_v4l2_close(exynos_camera, 0);
	exynos_v4l2_close(exynos_camera, 2);
}

// Params

int exynos_camera_params_init(struct exynos_camera *exynos_camera, int id)
{
	int rc;

	if (exynos_camera == NULL || id >= exynos_camera->config->presets_count)
		return -EINVAL;

	// Camera params
	exynos_camera->camera_rotation = exynos_camera->config->presets[id].rotation;
	exynos_camera->camera_hflip = exynos_camera->config->presets[id].hflip;
	exynos_camera->camera_vflip = exynos_camera->config->presets[id].vflip;
	exynos_camera->camera_picture_format = exynos_camera->config->presets[id].picture_format;
	exynos_camera->camera_focal_length = (int) (exynos_camera->config->presets[id].focal_length * 100);
	exynos_camera->camera_metering = exynos_camera->config->presets[id].metering;

	// Recording preview
	exynos_param_string_set(exynos_camera, "preferred-preview-size-for-video",
		exynos_camera->config->presets[id].params.preview_size);

	// Preview
	exynos_param_string_set(exynos_camera, "preview-size-values",
		exynos_camera->config->presets[id].params.preview_size_values);
	exynos_param_string_set(exynos_camera, "preview-size",
		exynos_camera->config->presets[id].params.preview_size);
	exynos_param_string_set(exynos_camera, "preview-format-values",
		exynos_camera->config->presets[id].params.preview_format_values);
	exynos_param_string_set(exynos_camera, "preview-format",
		exynos_camera->config->presets[id].params.preview_format);
	exynos_param_string_set(exynos_camera, "preview-frame-rate-values",
		exynos_camera->config->presets[id].params.preview_frame_rate_values);
	exynos_param_int_set(exynos_camera, "preview-frame-rate",
		exynos_camera->config->presets[id].params.preview_frame_rate);
	exynos_param_string_set(exynos_camera, "preview-fps-range-values",
		exynos_camera->config->presets[id].params.preview_fps_range_values);
	exynos_param_string_set(exynos_camera, "preview-fps-range",
		exynos_camera->config->presets[id].params.preview_fps_range);

	// Picture
	exynos_param_string_set(exynos_camera, "picture-size-values",
		exynos_camera->config->presets[id].params.picture_size_values);
	exynos_param_string_set(exynos_camera, "picture-size",
		exynos_camera->config->presets[id].params.picture_size);
	exynos_param_string_set(exynos_camera, "picture-format-values",
		exynos_camera->config->presets[id].params.picture_format_values);
	exynos_param_string_set(exynos_camera, "picture-format",
		exynos_camera->config->presets[id].params.picture_format);
	exynos_param_string_set(exynos_camera, "jpeg-thumbnail-size-values",
		exynos_camera->config->presets[id].params.jpeg_thumbnail_size_values);
	exynos_param_int_set(exynos_camera, "jpeg-thumbnail-width",
		exynos_camera->config->presets[id].params.jpeg_thumbnail_width);
	exynos_param_int_set(exynos_camera, "jpeg-thumbnail-height",
		exynos_camera->config->presets[id].params.jpeg_thumbnail_height);
	exynos_param_int_set(exynos_camera, "jpeg-thumbnail-quality",
		exynos_camera->config->presets[id].params.jpeg_thumbnail_quality);
	exynos_param_int_set(exynos_camera, "jpeg-quality",
		exynos_camera->config->presets[id].params.jpeg_quality);

	// Recording
	exynos_param_string_set(exynos_camera, "video-size",
		exynos_camera->config->presets[id].params.recording_size);
	exynos_param_string_set(exynos_camera, "video-size-values",
		exynos_camera->config->presets[id].params.recording_size_values);
	exynos_param_string_set(exynos_camera, "video-frame-format",
		exynos_camera->config->presets[id].params.recording_format);

	// Focus
	exynos_param_string_set(exynos_camera, "focus-mode",
		exynos_camera->config->presets[id].params.focus_mode);
	exynos_param_string_set(exynos_camera, "focus-mode-values",
		exynos_camera->config->presets[id].params.focus_mode_values);
	exynos_param_string_set(exynos_camera, "focus-distances",
		exynos_camera->config->presets[id].params.focus_distances);
	if (exynos_camera->config->presets[id].params.max_num_focus_areas > 0) {
		exynos_param_string_set(exynos_camera, "focus-areas",
			exynos_camera->config->presets[id].params.focus_areas);
		exynos_param_int_set(exynos_camera, "max-num-focus-areas",
			exynos_camera->config->presets[id].params.max_num_focus_areas);
	}

	// Zoom
	if (exynos_camera->config->presets[id].params.zoom_supported == 1) {
		exynos_param_string_set(exynos_camera, "zoom-supported", "true");

		if (exynos_camera->config->presets[id].params.smooth_zoom_supported == 1)
			exynos_param_string_set(exynos_camera, "smooth-zoom-supported", "true");

		if (exynos_camera->config->presets[id].params.zoom_ratios != NULL)
			exynos_param_string_set(exynos_camera, "zoom-ratios", exynos_camera->config->presets[id].params.zoom_ratios);

		exynos_param_int_set(exynos_camera, "zoom", exynos_camera->config->presets[id].params.zoom);
		exynos_param_int_set(exynos_camera, "max-zoom", exynos_camera->config->presets[id].params.max_zoom);

	} else {
		exynos_param_string_set(exynos_camera, "zoom-supported", "false");
	}

	// Flash
	exynos_param_string_set(exynos_camera, "flash-mode",
		exynos_camera->config->presets[id].params.flash_mode);
	exynos_param_string_set(exynos_camera, "flash-mode-values",
		exynos_camera->config->presets[id].params.flash_mode_values);

	// Exposure
	exynos_param_int_set(exynos_camera, "exposure-compensation",
		exynos_camera->config->presets[id].params.exposure_compensation);
	exynos_param_float_set(exynos_camera, "exposure-compensation-step",
		exynos_camera->config->presets[id].params.exposure_compensation_step);
	exynos_param_int_set(exynos_camera, "min-exposure-compensation",
		exynos_camera->config->presets[id].params.min_exposure_compensation);
	exynos_param_int_set(exynos_camera, "max-exposure-compensation",
		exynos_camera->config->presets[id].params.max_exposure_compensation);

	// WB
	exynos_param_string_set(exynos_camera, "whitebalance",
		exynos_camera->config->presets[id].params.whitebalance);
	exynos_param_string_set(exynos_camera, "whitebalance-values",
		exynos_camera->config->presets[id].params.whitebalance_values);

	// Scene mode
	exynos_param_string_set(exynos_camera, "scene-mode",
		exynos_camera->config->presets[id].params.scene_mode);
	exynos_param_string_set(exynos_camera, "scene-mode-values",
		exynos_camera->config->presets[id].params.scene_mode_values);

	// Effect
	exynos_param_string_set(exynos_camera, "effect",
		exynos_camera->config->presets[id].params.effect);
	exynos_param_string_set(exynos_camera, "effect-values",
		exynos_camera->config->presets[id].params.effect_values);

	// ISO
	exynos_param_string_set(exynos_camera, "iso",
		exynos_camera->config->presets[id].params.iso);
	exynos_param_string_set(exynos_camera, "iso-values",
		exynos_camera->config->presets[id].params.iso_values);

	// Camera
	exynos_param_float_set(exynos_camera, "focal-length",
		exynos_camera->config->presets[id].focal_length);
	exynos_param_float_set(exynos_camera, "horizontal-view-angle",
		exynos_camera->config->presets[id].horizontal_view_angle);
	exynos_param_float_set(exynos_camera, "vertical-view-angle",
		exynos_camera->config->presets[id].vertical_view_angle);

	rc = exynos_camera_params_apply(exynos_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to apply params", __func__);
		return -1;
	}

	return 0;
}

int exynos_camera_params_apply(struct exynos_camera *exynos_camera)
{
	char *recording_hint_string;
	char *recording_preview_size_string;

	char *preview_size_string;
	int preview_width = 0;
	int preview_height = 0;
	char *preview_format_string;
	int preview_format;
	float preview_format_bpp;
	int preview_fps;

	char *picture_size_string;
	int picture_width = 0;
	int picture_height = 0;
	char *picture_format_string;
	int picture_format;

	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;

	char *video_size_string;
	int recording_width = 0;
	int recording_height = 0;
	char *video_frame_format_string;
	int recording_format;
	int camera_sensor_mode;
	int camera_sensor_output_size;

	char *focus_mode_string;
	int focus_mode;
	char *focus_areas_string;
	int focus_left, focus_top, focus_right, focus_bottom, focus_weigth;
	int focus_x;
	int focus_y;

	char *zoom_supported_string;
	int zoom, max_zoom;

	char *flash_mode_string;
	int flash_mode;

	int exposure_compensation;
	int min_exposure_compensation;
	int max_exposure_compensation;

	char *whitebalance_string;
	int whitebalance;

	char *scene_mode_string;
	int scene_mode;

	char *effect_string;
	int effect;

	char *iso_string;
	int iso;

	int force ;

	int w, h;
	char *k;
	int rc;

	if (exynos_camera == NULL)
		return -EINVAL;

	if (!exynos_camera->preview_params_set) {
		ALOGE("%s: Setting preview params", __func__);
		exynos_camera->preview_params_set = 1;
		force = 1;
	} else {
		force = 0;
	}

	// Preview
	preview_size_string = exynos_param_string_get(exynos_camera, "preview-size");
	if (preview_size_string != NULL) {
		sscanf(preview_size_string, "%dx%d", &preview_width, &preview_height);

		if (preview_width != 0 && preview_width != exynos_camera->preview_width)
			exynos_camera->preview_width = preview_width;
		if (preview_height != 0 && preview_height != exynos_camera->preview_height)
			exynos_camera->preview_height = preview_height;
	}

	preview_format_string = exynos_param_string_get(exynos_camera, "preview-format");
	if (preview_format_string != NULL) {
		if (strcmp(preview_format_string, "yuv420sp") == 0) {
			preview_format = V4L2_PIX_FMT_NV21;
			preview_format_bpp = 1.5f;
		} else if (strcmp(preview_format_string, "yuv420p") == 0) {
			preview_format = V4L2_PIX_FMT_YUV420;
			preview_format_bpp = 1.5f;
		} else if (strcmp(preview_format_string, "rgb565") == 0) {
			preview_format = V4L2_PIX_FMT_RGB565;
			preview_format_bpp = 2.0f;
		} else if (strcmp(preview_format_string, "rgb8888") == 0) {
			preview_format = V4L2_PIX_FMT_RGB32;
			preview_format_bpp = 4.0f;
		} else {
			ALOGE("%s: Unsupported preview format: %s", __func__, preview_format_string);
			preview_format = V4L2_PIX_FMT_NV21;
			preview_format_bpp = 1.5f;
		}

		if (preview_format != exynos_camera->preview_format) {
			exynos_camera->preview_format = preview_format;
			exynos_camera->preview_format_bpp = preview_format_bpp;
		}
	}

	preview_fps = exynos_param_int_get(exynos_camera, "preview-frame-rate");
	if (preview_fps > 0)
		exynos_camera->preview_fps = preview_fps;
	else
		exynos_camera->preview_fps = 0;

	// Picture
	picture_size_string = exynos_param_string_get(exynos_camera, "picture-size");
	if (picture_size_string != NULL) {
		sscanf(picture_size_string, "%dx%d", &picture_width, &picture_height);

		if (picture_width != 0 && picture_width != exynos_camera->picture_width)
			exynos_camera->picture_width = picture_width;
		if (picture_height != 0 && picture_height != exynos_camera->picture_height)
			exynos_camera->picture_height = picture_height;
	}

	picture_format_string = exynos_param_string_get(exynos_camera, "picture-format");
	if (picture_format_string != NULL) {
		if (strcmp(picture_format_string, "jpeg") == 0) {
			picture_format = V4L2_PIX_FMT_JPEG;
		} else {
			ALOGE("%s: Unsupported picture format: %s", __func__, picture_format_string);
			picture_format = V4L2_PIX_FMT_JPEG;
		}

		if (picture_format != exynos_camera->picture_format)
			exynos_camera->picture_format = picture_format;
	}

	jpeg_thumbnail_width = exynos_param_int_get(exynos_camera, "jpeg-thumbnail-width");
	if (jpeg_thumbnail_width > 0)
		exynos_camera->jpeg_thumbnail_width = jpeg_thumbnail_width;

	jpeg_thumbnail_height = exynos_param_int_get(exynos_camera, "jpeg-thumbnail-height");
	if (jpeg_thumbnail_height > 0)
		exynos_camera->jpeg_thumbnail_height = jpeg_thumbnail_height;

	jpeg_thumbnail_quality = exynos_param_int_get(exynos_camera, "jpeg-thumbnail-quality");
	if (jpeg_thumbnail_quality > 0)
		exynos_camera->jpeg_thumbnail_quality = jpeg_thumbnail_quality;

	jpeg_quality = exynos_param_int_get(exynos_camera, "jpeg-quality");
	if (jpeg_quality <= 100 && jpeg_quality >= 0 && (jpeg_quality != exynos_camera->jpeg_quality || force)) {
		exynos_camera->jpeg_quality = jpeg_quality;
		rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAM_JPEG_QUALITY, jpeg_quality);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// Recording
	video_size_string = exynos_param_string_get(exynos_camera, "video-size");
	if (video_size_string == NULL)
		video_size_string = exynos_param_string_get(exynos_camera, "preview-size");

	if (video_size_string != NULL) {
		sscanf(video_size_string, "%dx%d", &recording_width, &recording_height);

		if (recording_width != 0 && recording_width != exynos_camera->recording_width)
			exynos_camera->recording_width = recording_width;
		if (recording_height != 0 && recording_height != exynos_camera->recording_height)
			exynos_camera->recording_height = recording_height;
	}

	video_frame_format_string = exynos_param_string_get(exynos_camera, "video-frame-format");
	if (video_frame_format_string != NULL) {
		if (strcmp(video_frame_format_string, "yuv420sp") == 0) {
			recording_format = V4L2_PIX_FMT_NV12;
		} else if (strcmp(video_frame_format_string, "yuv420p") == 0) {
			recording_format = V4L2_PIX_FMT_YUV420;
		} else if (strcmp(video_frame_format_string, "rgb565") == 0) {
			recording_format = V4L2_PIX_FMT_RGB565;
		} else if (strcmp(video_frame_format_string, "rgb8888") == 0) {
			recording_format = V4L2_PIX_FMT_RGB32;
		} else {
			ALOGE("%s: Unsupported recording format: %s", __func__, video_frame_format_string);
			recording_format = V4L2_PIX_FMT_NV12;
		}

		if (recording_format != exynos_camera->recording_format)
			exynos_camera->recording_format = recording_format;
	}

	recording_hint_string = exynos_param_string_get(exynos_camera, "recording-hint");
	if (recording_hint_string != NULL && strcmp(recording_hint_string, "true") == 0) {
		camera_sensor_mode = SENSOR_MOVIE;

		k = exynos_param_string_get(exynos_camera, "preview-size-values");
		while (recording_width != 0 && recording_height != 0) {
			if (k == NULL)
				break;

			sscanf(k, "%dx%d", &w, &h);

			// Look for same aspect ratio
			if ((recording_width * h) / recording_height == w) {
				preview_width = w;
				preview_height = h;
				break;
			}

			k = strchr(k, ',');
			if (k == NULL)
				break;

			k++;
		}

		if (preview_width != 0 && preview_width != exynos_camera->preview_width)
			exynos_camera->preview_width = preview_width;
		if (preview_height != 0 && preview_height != exynos_camera->preview_height)
			exynos_camera->preview_height = preview_height;

		camera_sensor_output_size = ((recording_width & 0xffff) << 16) | (recording_height & 0xffff);
		rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_SENSOR_OUTPUT_SIZE,
			camera_sensor_output_size);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	} else {
		camera_sensor_mode = SENSOR_CAMERA;
	}

	// Switching modes
	if (camera_sensor_mode != exynos_camera->camera_sensor_mode) {
		exynos_camera->camera_sensor_mode = camera_sensor_mode;
		rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_SENSOR_MODE, camera_sensor_mode);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// Focus
	focus_mode_string = exynos_param_string_get(exynos_camera, "focus-mode");
	if (focus_mode_string != NULL) {
		if (strcmp(focus_mode_string, "auto") == 0)
			focus_mode = FOCUS_MODE_AUTO;
		else if (strcmp(focus_mode_string, "infinity") == 0)
			focus_mode = FOCUS_MODE_INFINITY;
		else if (strcmp(focus_mode_string, "macro") == 0)
			focus_mode = FOCUS_MODE_MACRO;
		else if (strcmp(focus_mode_string, "fixed") == 0)
			focus_mode = FOCUS_MODE_FIXED;
		else if (strcmp(focus_mode_string, "facedetect") == 0)
			focus_mode = FOCUS_MODE_FACEDETECT;
		else if (strcmp(focus_mode_string, "continuous-video") == 0)
			focus_mode = FOCUS_MODE_CONTINOUS;
		else if (strcmp(focus_mode_string, "continuous-picture") == 0)
			focus_mode = FOCUS_MODE_CONTINOUS;
		else
			focus_mode = FOCUS_MODE_AUTO;

		if (focus_mode != exynos_camera->focus_mode || force) {
			exynos_camera->focus_mode = focus_mode;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_FOCUS_MODE, focus_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	focus_areas_string = exynos_param_string_get(exynos_camera, "focus-areas");
	if (focus_areas_string != NULL) {
		focus_left = focus_top = focus_right = focus_bottom = focus_weigth = 0;

		rc = sscanf(focus_areas_string, "(%d,%d,%d,%d,%d)",
			&focus_left, &focus_top, &focus_right, &focus_bottom, &focus_weigth);
		if (rc != 5)
			ALOGE("%s: sscanf failed!", __func__);

		focus_x = (((focus_left + focus_right) / 2) + 1000) * preview_width / 2000;
		focus_y =  (((focus_top + focus_bottom) / 2) + 1000) * preview_height / 2000;

		if (focus_x != exynos_camera->focus_x || force) {
			exynos_camera->focus_x = focus_x;

			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_OBJECT_POSITION_X, focus_x);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}

		if (focus_y != exynos_camera->focus_y || force) {
			exynos_camera->focus_y = focus_y;

			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_OBJECT_POSITION_Y, focus_y);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Zoom
	zoom_supported_string = exynos_param_string_get(exynos_camera, "zoom-supported");
	if (zoom_supported_string != NULL && strcmp(zoom_supported_string, "true") == 0) {
		zoom = exynos_param_int_get(exynos_camera, "zoom");
		max_zoom = exynos_param_int_get(exynos_camera, "max-zoom");
		if (zoom <= max_zoom && zoom >= 0 && (zoom != exynos_camera->zoom || force)) {
			exynos_camera->zoom = zoom;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_ZOOM, zoom);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}

	}

	// Flash
	flash_mode_string = exynos_param_string_get(exynos_camera, "flash-mode");
	if (flash_mode_string != NULL) {
		if (strcmp(flash_mode_string, "off") == 0)
			flash_mode = FLASH_MODE_OFF;
		else if (strcmp(flash_mode_string, "auto") == 0)
			flash_mode = FLASH_MODE_AUTO;
		else if (strcmp(flash_mode_string, "on") == 0)
			flash_mode = FLASH_MODE_ON;
		else if (strcmp(flash_mode_string, "torch") == 0)
			flash_mode = FLASH_MODE_TORCH;
		else
			flash_mode = FLASH_MODE_AUTO;

		if (flash_mode != exynos_camera->flash_mode || force) {
			exynos_camera->flash_mode = flash_mode;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_FLASH_MODE, flash_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Exposure
	exposure_compensation = exynos_param_int_get(exynos_camera, "exposure-compensation");
	min_exposure_compensation = exynos_param_int_get(exynos_camera, "min-exposure-compensation");
	max_exposure_compensation = exynos_param_int_get(exynos_camera, "max-exposure-compensation");

	if (exposure_compensation <= max_exposure_compensation && exposure_compensation >= min_exposure_compensation &&
		(exposure_compensation != exynos_camera->exposure_compensation || force)) {
		exynos_camera->exposure_compensation = exposure_compensation;
		rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_BRIGHTNESS, exposure_compensation);
		if (rc < 0)
			ALOGE("%s: s ctrl failed!", __func__);
	}

	// WB
	whitebalance_string = exynos_param_string_get(exynos_camera, "whitebalance");
	if (whitebalance_string != NULL) {
		if (strcmp(whitebalance_string, "auto") == 0)
			whitebalance = WHITE_BALANCE_AUTO;
		else if (strcmp(whitebalance_string, "incandescent") == 0)
			whitebalance = WHITE_BALANCE_TUNGSTEN;
		else if (strcmp(whitebalance_string, "fluorescent") == 0)
			whitebalance = WHITE_BALANCE_FLUORESCENT;
		else if (strcmp(whitebalance_string, "daylight") == 0)
			whitebalance = WHITE_BALANCE_SUNNY;
		else if (strcmp(whitebalance_string, "cloudy-daylight") == 0)
			whitebalance = WHITE_BALANCE_CLOUDY;
		else
			whitebalance = WHITE_BALANCE_AUTO;

		if (whitebalance != exynos_camera->whitebalance || force) {
			exynos_camera->whitebalance = whitebalance;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_WHITE_BALANCE, whitebalance);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Scene mode
	scene_mode_string = exynos_param_string_get(exynos_camera, "scene-mode");
	if (scene_mode_string != NULL) {
		if (strcmp(scene_mode_string, "auto") == 0)
			scene_mode = SCENE_MODE_NONE;
		else if (strcmp(scene_mode_string, "portrait") == 0)
			scene_mode = SCENE_MODE_PORTRAIT;
		else if (strcmp(scene_mode_string, "landscape") == 0)
			scene_mode = SCENE_MODE_LANDSCAPE;
		else if (strcmp(scene_mode_string, "night") == 0)
			scene_mode = SCENE_MODE_NIGHTSHOT;
		else if (strcmp(scene_mode_string, "beach") == 0)
			scene_mode = SCENE_MODE_BEACH_SNOW;
		else if (strcmp(scene_mode_string, "snow") == 0)
			scene_mode = SCENE_MODE_BEACH_SNOW;
		else if (strcmp(scene_mode_string, "sunset") == 0)
			scene_mode = SCENE_MODE_SUNSET;
		else if (strcmp(scene_mode_string, "fireworks") == 0)
			scene_mode = SCENE_MODE_FIREWORKS;
		else if (strcmp(scene_mode_string, "sports") == 0)
			scene_mode = SCENE_MODE_SPORTS;
		else if (strcmp(scene_mode_string, "party") == 0)
			scene_mode = SCENE_MODE_PARTY_INDOOR;
		else if (strcmp(scene_mode_string, "candlelight") == 0)
			scene_mode = SCENE_MODE_CANDLE_LIGHT;
		else if (strcmp(scene_mode_string, "dusk-dawn") == 0)
			scene_mode = SCENE_MODE_DUSK_DAWN;
		else if (strcmp(scene_mode_string, "fall-color") == 0)
			scene_mode = SCENE_MODE_FALL_COLOR;
		else if (strcmp(scene_mode_string, "back-light") == 0)
			scene_mode = SCENE_MODE_BACK_LIGHT;
		else if (strcmp(scene_mode_string, "text") == 0)
			scene_mode = SCENE_MODE_TEXT;
		else
			scene_mode = SCENE_MODE_NONE;

		if (scene_mode != exynos_camera->scene_mode || force) {
			exynos_camera->scene_mode = scene_mode;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_SCENE_MODE, scene_mode);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// Effect
	effect_string = exynos_param_string_get(exynos_camera, "effect");
	if (effect_string != NULL) {
		if (strcmp(effect_string, "auto") == 0)
			effect = IMAGE_EFFECT_NONE;
		else if (strcmp(effect_string, "mono") == 0)
			effect = IMAGE_EFFECT_BNW;
		else if (strcmp(effect_string, "negative") == 0)
			effect = IMAGE_EFFECT_NEGATIVE;
		else if (strcmp(effect_string, "sepia") == 0)
			effect = IMAGE_EFFECT_SEPIA;
		else if (strcmp(effect_string, "aqua") == 0)
			effect = IMAGE_EFFECT_AQUA;
		else
			effect = IMAGE_EFFECT_NONE;

		if (effect != exynos_camera->effect || force) {
			exynos_camera->effect = effect;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_EFFECT, effect);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	// ISO
	iso_string = exynos_param_string_get(exynos_camera, "iso");
	if (iso_string != NULL) {
		if (strcmp(iso_string, "auto") == 0)
			iso = ISO_AUTO;
		else if (strcmp(iso_string, "ISO50") == 0)
			iso = ISO_50;
		else if (strcmp(iso_string, "ISO100") == 0)
			iso = ISO_100;
		else if (strcmp(iso_string, "ISO200") == 0)
			iso = ISO_200;
		else if (strcmp(iso_string, "ISO400") == 0)
			iso = ISO_400;
		else if (strcmp(iso_string, "ISO800") == 0)
			iso = ISO_800;
		else
			iso = ISO_AUTO;

		if (iso != exynos_camera->iso || force) {
			exynos_camera->iso = iso;
			rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_ISO, iso);
			if (rc < 0)
				ALOGE("%s: s ctrl failed!", __func__);
		}
	}

	ALOGD("%s: Preview size: %dx%d, picture size: %dx%d, recording size: %dx%d",
		__func__, preview_width, preview_height, picture_width, picture_height,
		recording_width, recording_height);

	return 0;
}

// Picture

int exynos_camera_picture(struct exynos_camera *exynos_camera)
{
	camera_memory_t *data_memory = NULL;
	camera_memory_t *exif_data_memory = NULL;
	camera_memory_t *picture_data_memory = NULL;
	camera_memory_t *jpeg_thumbnail_data_memory = NULL;

	int camera_picture_format;
	int picture_width;
	int picture_height;
	int picture_format;

	int jpeg_thumbnail_width;
	int jpeg_thumbnail_height;
	int jpeg_thumbnail_quality;
	int jpeg_quality;

	int data_size;

	int offset = 0;
	void *picture_addr = NULL;
	int picture_size = 0;
	void *jpeg_thumbnail_addr = NULL;
	int jpeg_thumbnail_size = 0;

	int jpeg_fd;
	struct jpeg_enc_param jpeg_enc_params;
	enum jpeg_frame_format jpeg_in_format;
	enum jpeg_stream_format jpeg_out_format;
	enum jpeg_ret_type jpeg_result;
	void *jpeg_in_buffer;
	int jpeg_in_size;
	void *jpeg_out_buffer;
	int jpeg_out_size;

	exif_attribute_t exif_attributes;
	int exif_size = 0;

	int index;
	int rc;

	if (exynos_camera == NULL)
		return -EINVAL;

	picture_width = exynos_camera->picture_width;
	picture_height = exynos_camera->picture_height;
	picture_format = exynos_camera->picture_format;
	camera_picture_format = exynos_camera->camera_picture_format;
	jpeg_thumbnail_width = exynos_camera->jpeg_thumbnail_width;
	jpeg_thumbnail_height = exynos_camera->jpeg_thumbnail_height;
	jpeg_thumbnail_quality = exynos_camera->jpeg_thumbnail_quality;
	jpeg_quality = exynos_camera->jpeg_quality;

	if (camera_picture_format == 0)
		camera_picture_format = picture_format;

	// V4L2

	rc = exynos_v4l2_poll(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: poll failed!", __func__);
		return -1;
	} else if (rc == 0) {
		ALOGE("%s: poll timeout!", __func__);
		return -1;
	}

	rc = exynos_v4l2_streamoff_cap(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
		return -1;
	}

	index = exynos_v4l2_dqbuf_cap(exynos_camera, 0);
	if (index < 0) {
		ALOGE("%s: dqbuf failed!", __func__);
		return -1;
	}

	// This assumes that the output format is JPEG

	if (camera_picture_format == V4L2_PIX_FMT_JPEG) {
		rc = exynos_v4l2_g_ctrl(exynos_camera, 0, V4L2_CID_CAM_JPEG_MAIN_SIZE,
			&picture_size);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		rc = exynos_v4l2_g_ctrl(exynos_camera, 0, V4L2_CID_CAM_JPEG_MAIN_OFFSET,
			&offset);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		picture_addr = (void *) ((int) exynos_camera->picture_memory->data + offset);

		rc = exynos_v4l2_g_ctrl(exynos_camera, 0, V4L2_CID_CAM_JPEG_THUMB_SIZE,
			&jpeg_thumbnail_size);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		rc = exynos_v4l2_g_ctrl(exynos_camera, 0, V4L2_CID_CAM_JPEG_THUMB_OFFSET,
			&offset);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			return -1;
		}

		jpeg_thumbnail_addr = (void *) ((int) exynos_camera->picture_memory->data + offset);
	}

	// Thumbnail

	if (camera_picture_format == V4L2_PIX_FMT_JPEG && jpeg_thumbnail_addr != NULL && jpeg_thumbnail_size >= 0) {
		if (exynos_camera->callbacks.request_memory != NULL) {
			jpeg_thumbnail_data_memory =
				exynos_camera->callbacks.request_memory(-1,
					jpeg_thumbnail_size, 1, 0);
			if (jpeg_thumbnail_data_memory == NULL) {
				ALOGE("%s: thumb memory request failed!", __func__);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			goto error;
		}

		memcpy(jpeg_thumbnail_data_memory->data, jpeg_thumbnail_addr, jpeg_thumbnail_size);
	} else {
		jpeg_fd = api_jpeg_encode_init();
		if (jpeg_fd < 0) {
			ALOGE("%s: Failed to init JPEG", __func__);
			goto error;
		}

		switch (camera_picture_format) {
			case V4L2_PIX_FMT_RGB565:
				jpeg_in_format = RGB_565;
				jpeg_out_format = JPEG_420;
				jpeg_in_size = (picture_width * picture_height * 2);
				break;
			case V4L2_PIX_FMT_NV12:
			case V4L2_PIX_FMT_NV21:
			case V4L2_PIX_FMT_NV12T:
			case V4L2_PIX_FMT_YUV420:
				jpeg_in_format = YUV_420;
				jpeg_out_format = JPEG_420;
				jpeg_in_size = (picture_width * picture_height * 1.5);
				break;
			case V4L2_PIX_FMT_YUYV:
			case V4L2_PIX_FMT_UYVY:
			case V4L2_PIX_FMT_YUV422P:
			default:
				jpeg_in_format = YUV_422;
				jpeg_out_format = JPEG_422;
				jpeg_in_size = (picture_width * picture_height * 2);
				break;
		}

		memset(&jpeg_enc_params, 0, sizeof(jpeg_enc_params));

		jpeg_enc_params.width = jpeg_thumbnail_width;
		jpeg_enc_params.height = jpeg_thumbnail_height;
		jpeg_enc_params.in_fmt = jpeg_in_format;
		jpeg_enc_params.out_fmt = jpeg_out_format;

		if (jpeg_thumbnail_quality >= 90)
			jpeg_enc_params.quality = QUALITY_LEVEL_1;
		else if (jpeg_thumbnail_quality >= 80)
			jpeg_enc_params.quality = QUALITY_LEVEL_2;
		else if (jpeg_thumbnail_quality >= 70)
			jpeg_enc_params.quality = QUALITY_LEVEL_3;
		else
			jpeg_enc_params.quality = QUALITY_LEVEL_4;

		api_jpeg_set_encode_param(&jpeg_enc_params);

		jpeg_in_buffer = api_jpeg_get_encode_in_buf(jpeg_fd, jpeg_in_size);
		if (jpeg_in_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG in buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_buffer = api_jpeg_get_encode_out_buf(jpeg_fd);
		if (jpeg_out_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG out buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_in_buffer, exynos_camera->picture_memory->data, jpeg_in_size);

		jpeg_result = api_jpeg_encode_exe(jpeg_fd, &jpeg_enc_params);
		if (jpeg_result != JPEG_ENCODE_OK) {
			ALOGE("%s: Failed to encode JPEG", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_size = jpeg_enc_params.size;
		if (jpeg_out_size <= 0) {
			ALOGE("%s: Failed to get JPEG out size", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		if (exynos_camera->callbacks.request_memory != NULL) {
			jpeg_thumbnail_data_memory =
				exynos_camera->callbacks.request_memory(-1,
					jpeg_out_size, 1, 0);
			if (jpeg_thumbnail_data_memory == NULL) {
				ALOGE("%s: thumbnail memory request failed!", __func__);
				api_jpeg_encode_deinit(jpeg_fd);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_thumbnail_data_memory->data, jpeg_out_buffer, jpeg_out_size);
		jpeg_thumbnail_size = jpeg_out_size;

		api_jpeg_encode_deinit(jpeg_fd);
	}

	// Picture

	if (camera_picture_format == V4L2_PIX_FMT_JPEG && picture_addr != NULL && picture_size >= 0) {
		if (exynos_camera->callbacks.request_memory != NULL) {
			picture_data_memory =
				exynos_camera->callbacks.request_memory(-1,
					picture_size, 1, 0);
			if (picture_data_memory == NULL) {
				ALOGE("%s: picture memory request failed!", __func__);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			goto error;
		}

		memcpy(picture_data_memory->data, picture_addr, picture_size);
	} else {
		jpeg_fd = api_jpeg_encode_init();
		if (jpeg_fd < 0) {
			ALOGE("%s: Failed to init JPEG", __func__);
			goto error;
		}

		switch (camera_picture_format) {
			case V4L2_PIX_FMT_RGB565:
				jpeg_in_format = RGB_565;
				jpeg_out_format = JPEG_420;
				jpeg_in_size = (picture_width * picture_height * 2);
				break;
			case V4L2_PIX_FMT_NV12:
			case V4L2_PIX_FMT_NV21:
			case V4L2_PIX_FMT_NV12T:
			case V4L2_PIX_FMT_YUV420:
				jpeg_in_format = YUV_420;
				jpeg_out_format = JPEG_420;
				jpeg_in_size = (picture_width * picture_height * 1.5);
				break;
			case V4L2_PIX_FMT_YUYV:
			case V4L2_PIX_FMT_UYVY:
			case V4L2_PIX_FMT_YUV422P:
			default:
				jpeg_in_format = YUV_422;
				jpeg_out_format = JPEG_422;
				jpeg_in_size = (picture_width * picture_height * 2);
				break;
		}

		memset(&jpeg_enc_params, 0, sizeof(jpeg_enc_params));

		jpeg_enc_params.width = picture_width;
		jpeg_enc_params.height = picture_height;
		jpeg_enc_params.in_fmt = jpeg_in_format;
		jpeg_enc_params.out_fmt = jpeg_out_format;

		if (jpeg_quality >= 90)
			jpeg_enc_params.quality = QUALITY_LEVEL_1;
		else if (jpeg_quality >= 80)
			jpeg_enc_params.quality = QUALITY_LEVEL_2;
		else if (jpeg_quality >= 70)
			jpeg_enc_params.quality = QUALITY_LEVEL_3;
		else
			jpeg_enc_params.quality = QUALITY_LEVEL_4;

		api_jpeg_set_encode_param(&jpeg_enc_params);

		jpeg_in_buffer = api_jpeg_get_encode_in_buf(jpeg_fd, jpeg_in_size);
		if (jpeg_in_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG in buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_buffer = api_jpeg_get_encode_out_buf(jpeg_fd);
		if (jpeg_out_buffer == NULL) {
			ALOGE("%s: Failed to get JPEG out buffer", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(jpeg_in_buffer, exynos_camera->picture_memory->data, jpeg_in_size);

		jpeg_result = api_jpeg_encode_exe(jpeg_fd, &jpeg_enc_params);
		if (jpeg_result != JPEG_ENCODE_OK) {
			ALOGE("%s: Failed to encode JPEG", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		jpeg_out_size = jpeg_enc_params.size;
		if (jpeg_out_size <= 0) {
			ALOGE("%s: Failed to get JPEG out size", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		if (exynos_camera->callbacks.request_memory != NULL) {
			picture_data_memory =
				exynos_camera->callbacks.request_memory(-1,
					jpeg_out_size, 1, 0);
			if (picture_data_memory == NULL) {
				ALOGE("%s: picture memory request failed!", __func__);
				api_jpeg_encode_deinit(jpeg_fd);
				goto error;
			}
		} else {
			ALOGE("%s: No memory request function!", __func__);
			api_jpeg_encode_deinit(jpeg_fd);
			goto error;
		}

		memcpy(picture_data_memory->data, jpeg_out_buffer, jpeg_out_size);
		picture_size = jpeg_out_size;

		api_jpeg_encode_deinit(jpeg_fd);
	}

	// EXIF

	memset(&exif_attributes, 0, sizeof(exif_attributes));
	exynos_exif_attributes_create_static(exynos_camera, &exif_attributes);
	exynos_exif_attributes_create_params(exynos_camera, &exif_attributes);

	rc = exynos_exif_create(exynos_camera, &exif_attributes,
		jpeg_thumbnail_data_memory, jpeg_thumbnail_size,
		&exif_data_memory, &exif_size);
	if (rc < 0 || exif_data_memory == NULL || exif_size <= 0) {
		ALOGE("%s: EXIF create failed!", __func__);
		goto error;
	}

	data_size = exif_size + picture_size;

	if (exynos_camera->callbacks.request_memory != NULL) {
		data_memory =
			exynos_camera->callbacks.request_memory(-1,
				data_size, 1, 0);
		if (data_memory == NULL) {
			ALOGE("%s: data memory request failed!", __func__);
			goto error;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		goto error;
	}

	// Copy the first two bytes of the JPEG picture
	memcpy(data_memory->data, picture_data_memory->data, 2);

	// Copy the EXIF data
	memcpy((void *) ((int) data_memory->data + 2), exif_data_memory->data,
		exif_size);

	// Copy the JPEG picture
	memcpy((void *) ((int) data_memory->data + 2 + exif_size),
		(void *) ((int) picture_data_memory->data + 2), picture_size - 2);

	// Callbacks

	if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_SHUTTER) && EXYNOS_CAMERA_CALLBACK_DEFINED(notify))
		exynos_camera->callbacks.notify(CAMERA_MSG_SHUTTER, 0, 0,
			exynos_camera->callbacks.user);

	if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_RAW_IMAGE) && EXYNOS_CAMERA_CALLBACK_DEFINED(data) &&
		jpeg_thumbnail_data_memory != NULL)
		exynos_camera->callbacks.data(CAMERA_MSG_RAW_IMAGE,
			jpeg_thumbnail_data_memory, 0, NULL, exynos_camera->callbacks.user);

	if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_COMPRESSED_IMAGE) && EXYNOS_CAMERA_CALLBACK_DEFINED(data) &&
		data_memory != NULL)
		exynos_camera->callbacks.data(CAMERA_MSG_COMPRESSED_IMAGE,
			data_memory, 0, NULL, exynos_camera->callbacks.user);

	// Release memory

	if (jpeg_thumbnail_data_memory != NULL && jpeg_thumbnail_data_memory->release != NULL)
		jpeg_thumbnail_data_memory->release(jpeg_thumbnail_data_memory);

	if (picture_data_memory != NULL && picture_data_memory->release != NULL)
		picture_data_memory->release(picture_data_memory);

	if (exif_data_memory != NULL && exif_data_memory->release != NULL)
		exif_data_memory->release(exif_data_memory);

	if (data_memory != NULL && data_memory->release != NULL)
		data_memory->release(data_memory);

	return 0;

error:
	if (jpeg_thumbnail_data_memory != NULL && jpeg_thumbnail_data_memory->release != NULL)
		jpeg_thumbnail_data_memory->release(jpeg_thumbnail_data_memory);

	if (picture_data_memory != NULL && picture_data_memory->release != NULL)
		picture_data_memory->release(picture_data_memory);

	if (exif_data_memory != NULL && exif_data_memory->release != NULL)
		exif_data_memory->release(exif_data_memory);

	if (data_memory != NULL && data_memory->release != NULL)
		data_memory->release(data_memory);

	return -1;
}

void *exynos_camera_picture_thread(void *data)
{
	struct exynos_camera *exynos_camera;
	int rc;
	int i;

	if (data == NULL)
		return NULL;

	exynos_camera = (struct exynos_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	exynos_camera->picture_thread_running = 1;

	if (exynos_camera->picture_enabled == 1) {
		pthread_mutex_lock(&exynos_camera->picture_mutex);

		rc = exynos_camera_picture(exynos_camera);
		if (rc < 0) {
			ALOGE("%s: picture failed!", __func__);
			exynos_camera->picture_enabled = 0;
		}

		if (exynos_camera->picture_memory != NULL && exynos_camera->picture_memory->release != NULL) {
			exynos_camera->picture_memory->release(exynos_camera->picture_memory);
			exynos_camera->picture_memory = NULL;
		}

		pthread_mutex_unlock(&exynos_camera->picture_mutex);
	}

	exynos_camera->picture_thread_running = 0;
	exynos_camera->picture_enabled = 0;

	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int exynos_camera_picture_start(struct exynos_camera *exynos_camera)
{
	pthread_attr_t thread_attr;

	int width, height, format, camera_format;

	int fd;
	int rc;

	if (exynos_camera == NULL)
		return -EINVAL;

	// Stop preview thread
	exynos_camera_preview_stop(exynos_camera);

	width = exynos_camera->picture_width;
	height = exynos_camera->picture_height;
	format = exynos_camera->picture_format;
	camera_format = exynos_camera->camera_picture_format;

	// V4L2

	if (camera_format == 0)
		camera_format = format;

	rc = exynos_v4l2_enum_fmt_cap(exynos_camera, 0, camera_format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_fmt_pix_cap(exynos_camera, 0, width, height, camera_format, V4L2_PIX_FMT_MODE_CAPTURE);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		return -1;
	}

	// Only use 1 buffer
	rc = exynos_v4l2_reqbufs_cap(exynos_camera, 0, 1);
	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_querybuf_cap(exynos_camera, 0, 0);
	if (rc < 0) {
		ALOGE("%s: querybuf failed!", __func__);
		return -1;
	}

	exynos_camera->picture_buffer_length = rc;

	if (exynos_camera->callbacks.request_memory != NULL) {
		fd = exynos_v4l2_find_fd(exynos_camera, 0);
		if (fd < 0) {
			ALOGE("%s: Unable to find v4l2 fd", __func__);
			return -1;
		}

		if (exynos_camera->picture_memory != NULL && exynos_camera->picture_memory->release != NULL)
			exynos_camera->picture_memory->release(exynos_camera->picture_memory);

		exynos_camera->picture_memory =
			exynos_camera->callbacks.request_memory(fd,
				exynos_camera->picture_buffer_length, 1, 0);
		if (exynos_camera->picture_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			return -1;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		return -1;
	}

	rc = exynos_v4l2_qbuf_cap(exynos_camera, 0, 0);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_streamon_cap(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_CAPTURE, 0);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	// Thread

	if (exynos_camera->picture_thread_running) {
		ALOGE("Picture thread is already running!");
		return -1;
	}

	pthread_mutex_init(&exynos_camera->picture_mutex, NULL);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	exynos_camera->picture_enabled = 1;

	rc = pthread_create(&exynos_camera->picture_thread, &thread_attr,
		exynos_camera_picture_thread, (void *) exynos_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void exynos_camera_picture_stop(struct exynos_camera *exynos_camera)
{
	int rc;
	int i;

	if (exynos_camera == NULL)
		return;

	if (!exynos_camera->picture_enabled) {
		ALOGE("Picture was already stopped!");
		return;
	}

	pthread_mutex_lock(&exynos_camera->picture_mutex);

	// Disable picture to make the thread end
	exynos_camera->picture_enabled = 0;

	pthread_mutex_unlock(&exynos_camera->picture_mutex);

	// Wait for the thread to end
	for (i=0 ; i < 10 ; i++) {
		if (!exynos_camera->picture_thread_running)
			break;

		usleep(500);
	}

	pthread_mutex_destroy(&exynos_camera->picture_mutex);
}

// Auto-focus

void *exynos_camera_auto_focus_thread(void *data)
{
	struct exynos_camera *exynos_camera;
	int auto_focus_status = -1;
	int auto_focus_result = 0;
	int rc;
	int i;

	if (data == NULL)
		return NULL;

	exynos_camera = (struct exynos_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	exynos_camera->auto_focus_thread_running = 1;

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_SET_AUTO_FOCUS, AUTO_FOCUS_ON);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		auto_focus_result = 0;
		goto thread_exit;
	}

	while (exynos_camera->auto_focus_enabled == 1) {
		pthread_mutex_lock(&exynos_camera->auto_focus_mutex);

		rc = exynos_v4l2_g_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_AUTO_FOCUS_RESULT, &auto_focus_status);
		if (rc < 0) {
			ALOGE("%s: g ctrl failed!", __func__);
			auto_focus_result = 0;
			pthread_mutex_unlock(&exynos_camera->auto_focus_mutex);
			goto thread_exit;
		}

		if (auto_focus_status & M5MO_AF_STATUS_IN_PROGRESS) { // Progress
			usleep(10000); // Sleep 10 ms
		} else if (auto_focus_status == M5MO_AF_STATUS_SUCCESS || auto_focus_status == M5MO_AF_STATUS_1ST_SUCCESS) { // Success
			auto_focus_result = 1;
			pthread_mutex_unlock(&exynos_camera->auto_focus_mutex);
			goto thread_exit;
		} else {
			auto_focus_result = 0;
			ALOGE("AF failed or unknown result flag: 0x%x", auto_focus_status);
			pthread_mutex_unlock(&exynos_camera->auto_focus_mutex);
			goto thread_exit;
		}

		pthread_mutex_unlock(&exynos_camera->auto_focus_mutex);
	}

thread_exit:
	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CAMERA_SET_AUTO_FOCUS, AUTO_FOCUS_OFF);
	if (rc < 0)
		ALOGE("%s: s ctrl failed!", __func__);

	if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_FOCUS) && EXYNOS_CAMERA_CALLBACK_DEFINED(notify))
		exynos_camera->callbacks.notify(CAMERA_MSG_FOCUS,
			(int32_t) auto_focus_result, 0, exynos_camera->callbacks.user);
		
	exynos_camera->auto_focus_thread_running = 0;
	exynos_camera->auto_focus_enabled = 0;

	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int exynos_camera_auto_focus_start(struct exynos_camera *exynos_camera)
{
	pthread_attr_t thread_attr;
	int rc;

	if (exynos_camera == NULL)
		return -EINVAL;

	// Thread

	if (exynos_camera->auto_focus_thread_running) {
		ALOGE("Auto-focus thread is already running!");
		return -1;
	}

	pthread_mutex_init(&exynos_camera->auto_focus_mutex, NULL);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	exynos_camera->auto_focus_enabled = 1;

	rc = pthread_create(&exynos_camera->auto_focus_thread, &thread_attr,
		exynos_camera_auto_focus_thread, (void *) exynos_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void exynos_camera_auto_focus_stop(struct exynos_camera *exynos_camera)
{
	int rc;
	int i;

	if (exynos_camera == NULL)
		return;

	if (!exynos_camera->auto_focus_enabled) {
		ALOGE("Auto-focus was already stopped!");
		return;
	}

	pthread_mutex_lock(&exynos_camera->auto_focus_mutex);

	// Disable auto-focus to make the thread end
	exynos_camera->auto_focus_enabled = 0;

	pthread_mutex_unlock(&exynos_camera->auto_focus_mutex);

	// Wait for the thread to end
	for (i=0 ; i < 10 ; i++) {
		if (!exynos_camera->auto_focus_thread_running)
			break;

		usleep(500);
	}

	pthread_mutex_destroy(&exynos_camera->auto_focus_mutex);
}

// Preview

int exynos_camera_preview(struct exynos_camera *exynos_camera)
{
	buffer_handle_t *buffer;
	int stride;

	int width, height;
	float format_bpp;

	char *preview_format_string;
	int frame_size, offset;
	void *preview_data;
	void *window_data;

	unsigned int recording_y_addr;
	unsigned int recording_cbcr_addr;
	nsecs_t timestamp;
	struct exynos_camera_addrs *addrs;
	struct timespec ts;

	int index;
	int rc;
	int i;

	if (exynos_camera == NULL || exynos_camera->preview_memory == NULL ||
		exynos_camera->preview_window == NULL)
		return -EINVAL;

	timestamp = systemTime(1);

	// V4L2

	rc = exynos_v4l2_poll(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: poll failed!", __func__);
		return -1;
	} else if (rc == 0) {
		ALOGE("%s: poll timeout!", __func__);
		return -1;
	}

	index = exynos_v4l2_dqbuf_cap(exynos_camera, 0);
	if (index < 0 || index >= exynos_camera->preview_buffers_count) {
		ALOGE("%s: dqbuf failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_qbuf_cap(exynos_camera, 0, index);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		return -1;
	}

	// Preview window

	width = exynos_camera->preview_width;
	height = exynos_camera->preview_height;
	format_bpp = exynos_camera->preview_format_bpp;

	exynos_camera->preview_window->dequeue_buffer(exynos_camera->preview_window,
		&buffer, &stride);
	exynos_camera->gralloc->lock(exynos_camera->gralloc, *buffer, GRALLOC_USAGE_SW_WRITE_OFTEN,
		0, 0, width, height, &window_data);

	if (window_data == NULL) {
		ALOGE("%s: gralloc lock failed!", __func__);
		return -1;
	}

	frame_size = (int) ((float) width * (float) height * format_bpp);
	offset = index * frame_size;

	preview_data = (void *) ((int) exynos_camera->preview_memory->data + offset);
	memcpy(window_data, preview_data, frame_size);

	exynos_camera->gralloc->unlock(exynos_camera->gralloc, *buffer);
	exynos_camera->preview_window->enqueue_buffer(exynos_camera->preview_window,
		buffer);

	if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_PREVIEW_FRAME) && EXYNOS_CAMERA_CALLBACK_DEFINED(data)) {
		exynos_camera->callbacks.data(CAMERA_MSG_PREVIEW_FRAME,
			exynos_camera->preview_memory, index, NULL, exynos_camera->callbacks.user);
	}

	// Recording

	if (exynos_camera->recording_enabled && exynos_camera->recording_memory != NULL) {
		pthread_mutex_lock(&exynos_camera->recording_mutex);

		// V4L2

		rc = exynos_v4l2_poll(exynos_camera, 2);
		if (rc < 0) {
			ALOGE("%s: poll failed!", __func__);
			goto error_recording;
		} else if (rc == 0) {
			ALOGE("%s: poll timeout!", __func__);
			goto error_recording;
		}

		index = exynos_v4l2_dqbuf_cap(exynos_camera, 2);
		if (index < 0) {
			ALOGE("%s: dqbuf failed!", __func__);
			goto error_recording;
		}

		recording_y_addr = exynos_v4l2_s_ctrl(exynos_camera, 2, V4L2_CID_PADDR_Y, index);
		if (recording_y_addr == 0xffffffff) {
			ALOGE("%s: s ctrl failed!", __func__);
			goto error_recording;
		}

		recording_cbcr_addr = exynos_v4l2_s_ctrl(exynos_camera, 2, V4L2_CID_PADDR_CBCR, index);
		if (recording_cbcr_addr == 0xffffffff) {
			ALOGE("%s: s ctrl failed!", __func__);
			goto error_recording;
		}

		addrs = (struct exynos_camera_addrs *) exynos_camera->recording_memory->data;

		addrs[index].type = 0; // kMetadataBufferTypeCameraSource
		addrs[index].y = recording_y_addr;
		addrs[index].cbcr = recording_cbcr_addr;
		addrs[index].index = index;
		addrs[index].reserved = 0;

		pthread_mutex_unlock(&exynos_camera->recording_mutex);

		if (EXYNOS_CAMERA_MSG_ENABLED(CAMERA_MSG_VIDEO_FRAME) && EXYNOS_CAMERA_CALLBACK_DEFINED(data_timestamp)) {
			exynos_camera->callbacks.data_timestamp(timestamp, CAMERA_MSG_VIDEO_FRAME,
				exynos_camera->recording_memory, index, exynos_camera->callbacks.user);
		} else {
			rc = exynos_v4l2_qbuf_cap(exynos_camera, 2, index);
			if (rc < 0) {
				ALOGE("%s: qbuf failed!", __func__);
				return -1;
			}
		}
	}

	return 0;

error_recording:
	pthread_mutex_lock(&exynos_camera->recording_mutex);

	return -1;
}

void *exynos_camera_preview_thread(void *data)
{
	struct exynos_camera *exynos_camera;
	int rc;

	if (data == NULL)
		return NULL;

	exynos_camera = (struct exynos_camera *) data;

	ALOGE("%s: Starting thread", __func__);
	exynos_camera->preview_thread_running = 1;

	if (exynos_camera->preview_window == NULL) {
		// Lock preview lock mutex
		pthread_mutex_lock(&exynos_camera->preview_lock_mutex);
	}

	while (exynos_camera->preview_enabled == 1) {
		pthread_mutex_lock(&exynos_camera->preview_mutex);

		rc = exynos_camera_preview(exynos_camera);
		if (rc < 0) {
			ALOGE("%s: preview failed!", __func__);
			exynos_camera->preview_enabled = 0;
		}

		pthread_mutex_unlock(&exynos_camera->preview_mutex);
	}

	exynos_camera->preview_thread_running = 0;
	ALOGE("%s: Exiting thread", __func__);

	return NULL;
}

int exynos_camera_preview_start(struct exynos_camera *exynos_camera)
{
	struct v4l2_streamparm streamparm;
	int width, height, format;
	float format_bpp;
	int fps, frame_size;
	int fd;

	pthread_attr_t thread_attr;

	int rc;
	int i;

	if (exynos_camera == NULL)
		return -EINVAL;

	if (exynos_camera->preview_enabled) {
		ALOGE("Preview was already started!");
		return 0;
	}

	// V4L2

	format = exynos_camera->preview_format;

	rc = exynos_v4l2_enum_fmt_cap(exynos_camera, 0, format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		return -1;
	}

	width = exynos_camera->preview_width;
	height = exynos_camera->preview_height;
	format_bpp = exynos_camera->preview_format_bpp;

	rc = exynos_v4l2_s_fmt_pix_cap(exynos_camera, 0, width, height, format, V4L2_PIX_FMT_MODE_PREVIEW);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_CACHEABLE, 1);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	for (i=EXYNOS_CAMERA_MAX_BUFFERS_COUNT ; i >= EXYNOS_CAMERA_MIN_BUFFERS_COUNT ; i--) {
		rc = exynos_v4l2_reqbufs_cap(exynos_camera, 0, i);
		if (rc >= 0)
			break;
	}

	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		return -1;
	}

	exynos_camera->preview_buffers_count = rc;
	ALOGD("Found %d preview buffers available!", exynos_camera->preview_buffers_count);

	fps = exynos_camera->preview_fps;
	memset(&streamparm, 0, sizeof(streamparm));
	streamparm.parm.capture.timeperframe.numerator = 1;
	streamparm.parm.capture.timeperframe.denominator = fps;

	rc = exynos_v4l2_s_parm_cap(exynos_camera, 0, &streamparm);
	if (rc < 0) {
		ALOGE("%s: s parm failed!", __func__);
		return -1;
	}

	frame_size = (int) ((float) width * (float) height * format_bpp);
	for (i=0 ; i < exynos_camera->preview_buffers_count ; i++) {
		rc = exynos_v4l2_querybuf_cap(exynos_camera, 0, i);
		if (rc < 0) {
			ALOGE("%s: querybuf failed!", __func__);
			return -1;
		}

		if (rc != frame_size)
			ALOGE("%s: Frame size mismatch: %d/%d", __func__, frame_size, rc);
	}

	if (exynos_camera->callbacks.request_memory != NULL) {
		fd = exynos_v4l2_find_fd(exynos_camera, 0);
		if (fd < 0) {
			ALOGE("%s: Unable to find v4l2 fd", __func__);
			return -1;
		}

		if (exynos_camera->preview_memory != NULL && exynos_camera->preview_memory->release != NULL)
			exynos_camera->preview_memory->release(exynos_camera->preview_memory);

		exynos_camera->preview_memory =
			exynos_camera->callbacks.request_memory(fd,
				frame_size, exynos_camera->preview_buffers_count, 0);
		if (exynos_camera->preview_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			return -1;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		return -1;
	}

	for (i=0 ; i < exynos_camera->preview_buffers_count ; i++) {
		rc = exynos_v4l2_qbuf_cap(exynos_camera, 0, i);
		if (rc < 0) {
			ALOGE("%s: qbuf failed!", __func__);
			return -1;
		}
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_ROTATION,
		exynos_camera->camera_rotation);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_HFLIP,
		exynos_camera->camera_hflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 0, V4L2_CID_VFLIP,
		exynos_camera->camera_vflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		return -1;
	}

	rc = exynos_v4l2_streamon_cap(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		return -1;
	}

	// Thread

	pthread_mutex_init(&exynos_camera->preview_mutex, NULL);
	pthread_mutex_init(&exynos_camera->preview_lock_mutex, NULL);

	// Lock preview lock
	pthread_mutex_lock(&exynos_camera->preview_lock_mutex);

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);

	exynos_camera->preview_enabled = 1;

	rc = pthread_create(&exynos_camera->preview_thread, &thread_attr,
		exynos_camera_preview_thread, (void *) exynos_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to create thread", __func__);
		return -1;
	}

	return 0;
}

void exynos_camera_preview_stop(struct exynos_camera *exynos_camera)
{
	int rc;
	int i;

	if (exynos_camera == NULL)
		return;

	if (!exynos_camera->preview_enabled) {
		ALOGE("Preview was already stopped!");
		return;
	}

	exynos_camera->preview_enabled = 0;

	// Unlock preview lock
	pthread_mutex_unlock(&exynos_camera->preview_lock_mutex);

	pthread_mutex_lock(&exynos_camera->preview_mutex);

	// Wait for the thread to end
	for (i=0 ; i < 10 ; i++) {
		if (!exynos_camera->preview_thread_running)
			break;

		usleep(1000);
	}

	rc = exynos_v4l2_streamoff_cap(exynos_camera, 0);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
	}

	exynos_camera->preview_params_set = 0;

	if (exynos_camera->preview_memory != NULL && exynos_camera->preview_memory->release != NULL) {
		exynos_camera->preview_memory->release(exynos_camera->preview_memory);
		exynos_camera->preview_memory = NULL;
	}

	exynos_camera->preview_window = NULL;

	pthread_mutex_unlock(&exynos_camera->preview_mutex);

	pthread_mutex_destroy(&exynos_camera->preview_lock_mutex);
	pthread_mutex_destroy(&exynos_camera->preview_mutex);
}

// Recording

void exynos_camera_recording_frame_release(struct exynos_camera *exynos_camera, void *data)
{
	struct exynos_camera_addrs *addrs;
	int rc;

	if (exynos_camera == NULL || data == NULL)
		return;

	addrs = (struct exynos_camera_addrs *) data;
	if (addrs->index >= (unsigned int) exynos_camera->recording_buffers_count)
		return;

	pthread_mutex_lock(&exynos_camera->recording_mutex);

	rc = exynos_v4l2_qbuf_cap(exynos_camera, 2, addrs->index);
	if (rc < 0) {
		ALOGE("%s: qbuf failed!", __func__);
		goto error;
	}

error:
	pthread_mutex_unlock(&exynos_camera->recording_mutex);
}

int exynos_camera_recording_start(struct exynos_camera *exynos_camera)
{
	int width, height, format;
	int fd;

	int rc;
	int i;

	if (exynos_camera == NULL)
		return -EINVAL;

	if (exynos_camera->recording_enabled) {
		ALOGE("Recording was already started!");
		return 0;
	}

	pthread_mutex_lock(&exynos_camera->preview_mutex);

	// V4L2

	format = exynos_camera->recording_format;

	rc = exynos_v4l2_enum_fmt_cap(exynos_camera, 2, format);
	if (rc < 0) {
		ALOGE("%s: enum fmt failed!", __func__);
		goto error;
	}

	width = exynos_camera->recording_width;
	height = exynos_camera->recording_height;

	rc = exynos_v4l2_s_fmt_pix_cap(exynos_camera, 2, width, height, format, V4L2_PIX_FMT_MODE_CAPTURE);
	if (rc < 0) {
		ALOGE("%s: s fmt failed!", __func__);
		goto error;
	}

	for (i=EXYNOS_CAMERA_MAX_BUFFERS_COUNT ; i >= EXYNOS_CAMERA_MIN_BUFFERS_COUNT ; i--) {
		rc = exynos_v4l2_reqbufs_cap(exynos_camera, 2, i);
		if (rc >= 0)
			break;
	}

	if (rc < 0) {
		ALOGE("%s: reqbufs failed!", __func__);
		goto error;
	}

	exynos_camera->recording_buffers_count = rc;
	ALOGD("Found %d recording buffers available!", exynos_camera->recording_buffers_count);

	for (i=0 ; i < exynos_camera->recording_buffers_count ; i++) {
		rc = exynos_v4l2_querybuf_cap(exynos_camera, 2, i);
		if (rc < 0) {
			ALOGE("%s: querybuf failed!", __func__);
			goto error;
		}
	}

	if (exynos_camera->callbacks.request_memory != NULL) {
		if (exynos_camera->recording_memory != NULL && exynos_camera->recording_memory->release != NULL)
			exynos_camera->recording_memory->release(exynos_camera->recording_memory);

		exynos_camera->recording_memory =
			exynos_camera->callbacks.request_memory(-1, sizeof(struct exynos_camera_addrs),
				exynos_camera->recording_buffers_count, 0);
		if (exynos_camera->recording_memory == NULL) {
			ALOGE("%s: memory request failed!", __func__);
			goto error;
		}
	} else {
		ALOGE("%s: No memory request function!", __func__);
		goto error;
	}

	for (i=0 ; i < exynos_camera->recording_buffers_count ; i++) {
		rc = exynos_v4l2_qbuf_cap(exynos_camera, 2, i);
		if (rc < 0) {
			ALOGE("%s: qbuf failed!", __func__);
			goto error;
		}
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 2, V4L2_CID_ROTATION,
		exynos_camera->camera_rotation);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 2, V4L2_CID_HFLIP,
		exynos_camera->camera_hflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = exynos_v4l2_s_ctrl(exynos_camera, 2, V4L2_CID_VFLIP,
		exynos_camera->camera_vflip);
	if (rc < 0) {
		ALOGE("%s: s ctrl failed!", __func__);
		goto error;
	}

	rc = exynos_v4l2_streamon_cap(exynos_camera, 2);
	if (rc < 0) {
		ALOGE("%s: streamon failed!", __func__);
		goto error;
	}

	pthread_mutex_init(&exynos_camera->recording_mutex, NULL);

	exynos_camera->recording_enabled = 1;

	pthread_mutex_unlock(&exynos_camera->preview_mutex);

	return 0;
error:
	pthread_mutex_unlock(&exynos_camera->preview_mutex);

	return -1;
}

void exynos_camera_recording_stop(struct exynos_camera *exynos_camera)
{
	int rc;

	if (exynos_camera == NULL)
		return;

	if (!exynos_camera->recording_enabled) {
		ALOGE("Recording was already stopped!");
		return;
	}

	exynos_camera->recording_enabled = 0;

	pthread_mutex_lock(&exynos_camera->preview_mutex);

	rc = exynos_v4l2_streamoff_cap(exynos_camera, 2);
	if (rc < 0) {
		ALOGE("%s: streamoff failed!", __func__);
	}

	if (exynos_camera->recording_memory != NULL && exynos_camera->recording_memory->release != NULL) {
		exynos_camera->recording_memory->release(exynos_camera->recording_memory);
		exynos_camera->recording_memory = NULL;
	}

	pthread_mutex_unlock(&exynos_camera->preview_mutex);

	pthread_mutex_destroy(&exynos_camera->recording_mutex);
}

/*
 * Exynos Camera OPS
 */

int exynos_camera_set_preview_window(struct camera_device *dev,
	struct preview_stream_ops *w)
{
	struct exynos_camera *exynos_camera;

	int width, height, format, hal_format;

	buffer_handle_t *buffer;
	int stride;
	void *addr = NULL;

	int rc;

	ALOGD("%s(%p, %p)", __func__, dev, w);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	if (w == NULL)
		return 0;

	exynos_camera->preview_window = w;

	if (w->set_buffer_count == NULL || w->set_usage == NULL || w->set_buffers_geometry == NULL)
		return -EINVAL;

        if (exynos_camera->preview_width == 640 || exynos_camera->preview_buffers_count <= 0) {
		ALOGE("%s: Invalid preview buffers count", __func__);
		exynos_camera->preview_buffers_count = EXYNOS_CAMERA_MAX_BUFFERS_COUNT;
	}

	rc = w->set_buffer_count(w, exynos_camera->preview_buffers_count);
	if (rc) {
		ALOGE("%s: Unable to set buffer count (%d)", __func__,
			exynos_camera->preview_buffers_count);
		return -1;
	}

	rc = w->set_usage(w, GRALLOC_USAGE_SW_WRITE_OFTEN);
	if (rc) {
		ALOGE("%s: Unable to set usage", __func__);
		return -1;
	}

	width = exynos_camera->preview_width;
	height = exynos_camera->preview_height;
	format = exynos_camera->preview_format;

	switch (format) {
		case V4L2_PIX_FMT_NV21:
			hal_format = HAL_PIXEL_FORMAT_YCrCb_420_SP;
			break;
		case V4L2_PIX_FMT_YUV420:
			hal_format = HAL_PIXEL_FORMAT_YV12;
			break;
		case V4L2_PIX_FMT_RGB565:
			hal_format = HAL_PIXEL_FORMAT_RGB_565;
			break;
		case V4L2_PIX_FMT_RGB32:
			hal_format = HAL_PIXEL_FORMAT_RGBX_8888;
			break;
		default:
			hal_format = HAL_PIXEL_FORMAT_YCrCb_420_SP;
			break;
	}

	rc = w->set_buffers_geometry(w, width, height, hal_format);
	if (rc) {
		ALOGE("%s: Unable to set buffers geometry", __func__);
		return -1;
	}

	// Unlock preview lock
	pthread_mutex_unlock(&exynos_camera->preview_lock_mutex);

	return 0;
}

void exynos_camera_set_callbacks(struct camera_device *dev,
	camera_notify_callback notify_cb,
	camera_data_callback data_cb,
	camera_data_timestamp_callback data_cb_timestamp,
	camera_request_memory get_memory,
	void *user)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p, %p)", __func__, dev, user);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera->callbacks.notify = notify_cb;
	exynos_camera->callbacks.data = data_cb;
	exynos_camera->callbacks.data_timestamp = data_cb_timestamp;
	exynos_camera->callbacks.request_memory = get_memory;
	exynos_camera->callbacks.user = user;
}

void exynos_camera_enable_msg_type(struct camera_device *dev, int32_t msg_type)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p, %d)", __func__, dev, msg_type);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera->messages_enabled |= msg_type;
}

void exynos_camera_disable_msg_type(struct camera_device *dev, int32_t msg_type)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p, %d)", __func__, dev, msg_type);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera->messages_enabled &= ~msg_type;
}

int exynos_camera_msg_type_enabled(struct camera_device *dev, int32_t msg_type)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p, %d)", __func__, dev, msg_type);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera->messages_enabled & msg_type;
}

int exynos_camera_start_preview(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera_preview_start(exynos_camera);
}

void exynos_camera_stop_preview(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera_preview_stop(exynos_camera);
}

int exynos_camera_preview_enabled(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera->preview_enabled;
}

int exynos_camera_store_meta_data_in_buffers(struct camera_device *dev,
	int enable)
{
	ALOGD("%s(%p, %d)", __func__, dev, enable);

	if (!enable) {
		ALOGE("%s: Cannot disable meta-data in buffers!", __func__);
		return -1;
	}

	return 0;
}

int exynos_camera_start_recording(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera_recording_start(exynos_camera);
}

void exynos_camera_stop_recording(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera_recording_stop(exynos_camera);
}

int exynos_camera_recording_enabled(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera->recording_enabled;
}

void exynos_camera_release_recording_frame(struct camera_device *dev,
	const void *opaque)
{
	struct exynos_camera *exynos_camera;

	ALOGV("%s(%p, %p)", __func__, dev, opaque);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera_recording_frame_release(exynos_camera, (void *) opaque);
}

int exynos_camera_auto_focus(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera_auto_focus_start(exynos_camera);
}

int exynos_camera_cancel_auto_focus(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera_auto_focus_stop(exynos_camera);

	return 0;
}

int exynos_camera_take_picture(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	return exynos_camera_picture_start(exynos_camera);
}

int exynos_camera_cancel_picture(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	exynos_camera_picture_stop(exynos_camera);

	return 0;
}

int exynos_camera_set_parameters(struct camera_device *dev,
	const char *params)
{
	struct exynos_camera *exynos_camera;
	int rc;

	ALOGD("%s(%p, %s)", __func__, dev, params);

	if (dev == NULL || dev->priv == NULL || params == NULL)
		return -EINVAL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	rc = exynos_params_string_set(exynos_camera, (char *) params);
	if (rc < 0) {
		ALOGE("%s: Unable to set params string", __func__);
		return -1;
	}

	rc = exynos_camera_params_apply(exynos_camera);
	if (rc < 0) {
		ALOGE("%s: Unable to apply params", __func__);
		return -1;
	}

	return 0;
}

char *exynos_camera_get_parameters(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;
	char *params;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return NULL;

	exynos_camera = (struct exynos_camera *) dev->priv;

	params = exynos_params_string_get(exynos_camera);
	if (params == NULL) {
		ALOGE("%s: Couldn't find any param", __func__);
		return strdup("");
	}

	return params;
}

void exynos_camera_put_parameters(struct camera_device *dev, char *params)
{
	ALOGD("%s(%p)", __func__, dev);

	if (params != NULL)
		free(params);
}

int exynos_camera_send_command(struct camera_device *dev,
	int32_t cmd, int32_t arg1, int32_t arg2)
{
	ALOGD("%s(%p, %d, %d, %d)", __func__, dev, cmd, arg1, arg2);

	return 0;
}

void exynos_camera_release(struct camera_device *dev)
{
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, dev);

	if (dev == NULL || dev->priv == NULL)
		return;

	exynos_camera = (struct exynos_camera *) dev->priv;

	if (exynos_camera->preview_memory != NULL && exynos_camera->preview_memory->release != NULL) {
		exynos_camera->preview_memory->release(exynos_camera->preview_memory);
		exynos_camera->preview_memory = NULL;
	}

	if (exynos_camera->picture_memory != NULL && exynos_camera->picture_memory->release != NULL) {
		exynos_camera->picture_memory->release(exynos_camera->picture_memory);
		exynos_camera->picture_memory = NULL;
	}

	exynos_camera_deinit(exynos_camera);
}

int exynos_camera_dump(struct camera_device *dev, int fd)
{
	ALOGD("%s(%p, %d)", __func__, dev, fd);

	return 0;
}

/*
 * Interface
 */

struct camera_device_ops exynos_camera_ops = {
	.set_preview_window = exynos_camera_set_preview_window,
	.set_callbacks = exynos_camera_set_callbacks,
	.enable_msg_type = exynos_camera_enable_msg_type,
	.disable_msg_type = exynos_camera_disable_msg_type,
	.msg_type_enabled = exynos_camera_msg_type_enabled,
	.start_preview = exynos_camera_start_preview,
	.stop_preview = exynos_camera_stop_preview,
	.preview_enabled = exynos_camera_preview_enabled,
	.store_meta_data_in_buffers = exynos_camera_store_meta_data_in_buffers,
	.start_recording = exynos_camera_start_recording,
	.stop_recording = exynos_camera_stop_recording,
	.recording_enabled = exynos_camera_recording_enabled,
	.release_recording_frame = exynos_camera_release_recording_frame,
	.auto_focus = exynos_camera_auto_focus,
	.cancel_auto_focus = exynos_camera_cancel_auto_focus,
	.take_picture = exynos_camera_take_picture,
	.cancel_picture = exynos_camera_cancel_picture,
	.set_parameters = exynos_camera_set_parameters,
	.get_parameters = exynos_camera_get_parameters,
	.put_parameters = exynos_camera_put_parameters,
	.send_command = exynos_camera_send_command,
	.release = exynos_camera_release,
	.dump = exynos_camera_dump,
};

int exynos_camera_close(hw_device_t *device)
{
	struct camera_device *camera_device;
	struct exynos_camera *exynos_camera;

	ALOGD("%s(%p)", __func__, device);

	if (device == NULL)
		return -EINVAL;

	camera_device = (struct camera_device *) device;

	if (camera_device->priv != NULL) {
		free(camera_device->priv);
	}

	free(camera_device);

	return 0;
}

int exynos_camera_open(const struct hw_module_t* module, const char *camera_id,
	struct hw_device_t** device)
{
	struct camera_device *camera_device = NULL;
	struct exynos_camera *exynos_camera = NULL;
	int id;
	int rc;

	ALOGD("%s(%p, %s, %p)", __func__, module, camera_id, device);

	if (module == NULL || camera_id == NULL || device == NULL)
		return -EINVAL;

	id = atoi(camera_id);
	if (id < 0)
		return -EINVAL;

	exynos_camera = calloc(1, sizeof(struct exynos_camera));
	exynos_camera->config = exynos_camera_config;

	if (exynos_camera->config->presets_count > EXYNOS_CAMERA_MAX_PRESETS_COUNT ||
		exynos_camera->config->v4l2_nodes_count > EXYNOS_CAMERA_MAX_V4L2_NODES_COUNT)
		goto error_preset;

	if (id >= exynos_camera->config->presets_count)
		goto error_preset;

	rc = exynos_camera_init(exynos_camera, id);
	if (rc < 0) {
		ALOGE("%s: Unable to init camera", __func__);
		goto error;
	}

	camera_device = calloc(1, sizeof(struct camera_device));
	camera_device->common.tag = HARDWARE_DEVICE_TAG;
	camera_device->common.version = 0;
	camera_device->common.module = (struct hw_module_t *) module;
	camera_device->common.close = exynos_camera_close;

	camera_device->ops = &exynos_camera_ops;
	camera_device->priv = exynos_camera;

	*device = (struct hw_device_t *) &(camera_device->common);

	return 0;

error:
	exynos_camera_deinit(exynos_camera);

error_device:
	if (camera_device != NULL)
		free(camera_device);

error_preset:
	if (exynos_camera != NULL)
		free(exynos_camera);

	return -1;
}

int exynos_camera_get_number_of_cameras(void)
{
	ALOGD("%s()", __func__);

	if (exynos_camera_config == NULL || exynos_camera_config->presets == NULL) {
		ALOGE("%s: Unable to find proper camera config", __func__);
		return -1;
	}

	return exynos_camera_config->presets_count;
}

int exynos_camera_get_camera_info(int id, struct camera_info *info)
{
	ALOGD("%s(%d, %p)", __func__, id, info);

	if (id < 0 || info == NULL)
		return -EINVAL;

	if (exynos_camera_config == NULL || exynos_camera_config->presets == NULL) {
		ALOGE("%s: Unable to find proper camera config", __func__);
		return -1;
	}

	if (id >= exynos_camera_config->presets_count)
		return -EINVAL;

	ALOGD("Selected camera: %s", exynos_camera_config->presets[id].name);

	info->facing = exynos_camera_config->presets[id].facing;
	info->orientation = exynos_camera_config->presets[id].orientation;

	return 0;
}

struct hw_module_methods_t exynos_camera_module_methods = {
	.open = exynos_camera_open,
};

struct camera_module HAL_MODULE_INFO_SYM = {
	.common = {
		.tag = HARDWARE_MODULE_TAG,
		.hal_api_version = HARDWARE_HAL_API_VERSION,
		.module_api_version = CAMERA_MODULE_API_VERSION_1_0,
		.id = CAMERA_HARDWARE_MODULE_ID,
		.name = "Exynos Camera",
		.author = "Paul Kocialkowski",
		.methods = &exynos_camera_module_methods,
	},
	.get_number_of_cameras = exynos_camera_get_number_of_cameras,
	.get_camera_info = exynos_camera_get_camera_info,
};
