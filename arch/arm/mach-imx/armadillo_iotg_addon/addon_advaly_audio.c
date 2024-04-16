// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 ADVALY SYSTEM Inc. All Rights Reserved.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_fdt.h>
#include <linux/of_gpio.h>

#include "armadillo_iotg_std_addon.h"

extern_dtb(addon_advaly_audio_iotg_g3_intf1);
extern_dtb(addon_advaly_audio_iotg_g3_intf2);

#define PIN_MIC_N_LINE	(24)
#define PIN_MUTE_N	(25)
#define PIN_GAIN3	(35)
#define PIN_GAIN2	(36)
#define PIN_GAIN1	(37)
#define PIN_DIPSW3	(47)
#define PIN_DIPSW2	(48)
#define PIN_DIPSW1	(49)

static struct armadillo_iotg_addon *addon;

static int advaly_audio_setup_pin(struct gpio agpio,
				 struct addon_device *adev)
{
	char label[64];
	int ret;

	ret = devm_gpio_request_one(addon->dev,
				    agpio.gpio,
				    agpio.flags,
				    agpio.label);
	if (ret) {
		dev_err(addon->dev,
			"failed to request gpio %s\n",
			agpio.label);
		return ret;
	}

	snprintf(label, sizeof(label), "%s_INTF%d", agpio.label, adev->intf);
	gpio_export_link(addon->dev, label, agpio.gpio);

	return 0;
}

int addon_setup_advaly_audio(struct addon_device *adev)
{
	void *begin;
	size_t size;
	const struct gpio pins[] = {
		{ adev->gpios[PIN_MIC_N_LINE - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "MIC_N_LINE" },
		{ adev->gpios[PIN_MUTE_N - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "MUTE_N" },
		{ adev->gpios[PIN_GAIN1 - 1], GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT, "GAIN1" },
		{ adev->gpios[PIN_GAIN2 - 1], GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT, "GAIN2" },
		{ adev->gpios[PIN_GAIN3 - 1], GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT, "GAIN3" },
		{ adev->gpios[PIN_DIPSW1 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DIPSW1" },
		{ adev->gpios[PIN_DIPSW2 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DIPSW2" },
		{ adev->gpios[PIN_DIPSW3 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DIPSW3" },
	};
	int ret;
	int i;

	addon = to_addon(adev);

	switch (adev->intf) {
	case ADDON_INTERFACE1:
		switch (addon->type) {
		case ADDON_BOARD_TYPE_IOTG:
			begin = dtb_begin(addon_advaly_audio_iotg_g3_intf1);
			size = dtb_size(addon_advaly_audio_iotg_g3_intf1);
			break;
		case ADDON_BOARD_TYPE_X1:
		default:
			return -ENODEV;
		};
		break;
	case ADDON_INTERFACE2:
		switch (addon->type) {
		case ADDON_BOARD_TYPE_IOTG:
			begin = dtb_begin(addon_advaly_audio_iotg_g3_intf2);
			size = dtb_size(addon_advaly_audio_iotg_g3_intf2);
			break;
		default:
			return -ENODEV;
		};
		break;
	default:
		BUG();
	};
	armadillo_iotg_addon_dt_overlay(addon->dev, begin, size);

	for (i = 0; i < ARRAY_SIZE(pins); i++) {
		ret = advaly_audio_setup_pin(pins[i], adev);
	}

	return ret;
}
