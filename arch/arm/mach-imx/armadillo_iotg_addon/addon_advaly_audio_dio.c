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

extern_dtb(addon_advaly_audio_dio_iotg_g3_intf1);

#define PIN_SW_AUDIO_SEL0	(3)
#define PIN_SW_AUDIO_SEL1	(4)
#define PIN_AUDIO_SEL0		(32)
#define PIN_AUDIO_SEL1		(33)
#define PIN_DI0			(38)
#define PIN_DI1			(39)
#define PIN_DI2			(35)
#define PIN_DI3			(37)
#define PIN_DO0			(42)
#define PIN_DO1			(43)
#define PIN_DO2			(44)
#define PIN_DO3			(45)
#define PIN_DO4			(46)
#define PIN_DO5			(47)
#define PIN_DO6			(48)
#define PIN_DO7			(49)
#define PIN_DO_EN		(41)

enum sw_index {
	INDEX_SW_AUDIO_SEL0,
	INDEX_SW_AUDIO_SEL1,
};

enum audio_sel_index {
	INDEX_AUDIO_SEL0,
	INDEX_AUDIO_SEL1,
};

enum audio_mode {
	AUDIO_MODE_MONO,
	AUDIO_MODE_INVALID,
	AUDIO_MODE_STEREO,
	AUDIO_MODE_BALANCE,
};

static struct armadillo_iotg_addon *addon;

static int advaly_audio_dio_setup_gpio(struct gpio agpio,
				 struct addon_device *adev)
{
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

	return 0;
}

static int advaly_audio_dio_setup_dio(struct gpio agpio,
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

int addon_setup_advaly_audio_dio(struct addon_device *adev)
{
	void *begin;
	size_t size;
	const struct gpio dio_gpios[] = {
		{ adev->gpios[PIN_DI0 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DI0" },
		{ adev->gpios[PIN_DI1 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DI1" },
		{ adev->gpios[PIN_DI2 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DI2" },
		{ adev->gpios[PIN_DI3 - 1], GPIOF_DIR_IN | GPIOF_EXPORT, "DI3" },
		{ adev->gpios[PIN_DO0 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO0" },
		{ adev->gpios[PIN_DO1 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO1" },
		{ adev->gpios[PIN_DO2 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO2" },
		{ adev->gpios[PIN_DO3 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO3" },
		{ adev->gpios[PIN_DO4 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO4" },
		{ adev->gpios[PIN_DO5 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO5" },
		{ adev->gpios[PIN_DO6 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO6" },
		{ adev->gpios[PIN_DO7 - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO7" },
		{ adev->gpios[PIN_DO_EN - 1], GPIOF_OUT_INIT_LOW | GPIOF_EXPORT, "DO_EN" },
	};
	const struct gpio sw_gpios[] = {
		{ adev->gpios[PIN_SW_AUDIO_SEL0 - 1], GPIOF_DIR_IN, NULL },
		{ adev->gpios[PIN_SW_AUDIO_SEL1 - 1], GPIOF_DIR_IN, NULL },
	};
	const struct gpio audio_sel_gpios[] = {
		{ adev->gpios[PIN_AUDIO_SEL0 - 1], GPIOF_OUT_INIT_LOW, NULL },
		{ adev->gpios[PIN_AUDIO_SEL1 - 1], GPIOF_OUT_INIT_LOW, NULL },
	};
	int audio_mode;
	int ret;
	int i;

	addon = to_addon(adev);

	switch (adev->intf) {
	case ADDON_INTERFACE1:
		switch (addon->type) {
		case ADDON_BOARD_TYPE_IOTG:
			begin = dtb_begin(addon_advaly_audio_dio_iotg_g3_intf1);
			size = dtb_size(addon_advaly_audio_dio_iotg_g3_intf1);
			break;
		case ADDON_BOARD_TYPE_X1:
		default:
			return -ENODEV;
		};
		break;
	case ADDON_INTERFACE2:
		return -ENODEV;
	default:
		BUG();
	};
	armadillo_iotg_addon_dt_overlay(addon->dev, begin, size);

	for (i = 0; i < ARRAY_SIZE(dio_gpios); i++) {
		ret = advaly_audio_dio_setup_dio(dio_gpios[i], adev);
		if (ret)
			goto err;
	}

	for (i = 0; i < ARRAY_SIZE(sw_gpios); i++) {
		ret = advaly_audio_dio_setup_gpio(sw_gpios[i], adev);
		if (ret)
			goto err;
	}

	for (i = 0; i < ARRAY_SIZE(audio_sel_gpios); i++) {
		ret = advaly_audio_dio_setup_gpio(audio_sel_gpios[i], adev);
		if (ret)
			goto err;
	}

	audio_mode = !gpio_get_value(sw_gpios[INDEX_SW_AUDIO_SEL0].gpio)
			| (!gpio_get_value(sw_gpios[INDEX_SW_AUDIO_SEL1].gpio) << 1);
	switch (audio_mode) {
	case AUDIO_MODE_STEREO:
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL0].gpio, 0);
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL1].gpio, 1);
		break;
	case AUDIO_MODE_BALANCE:
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL0].gpio, 1);
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL1].gpio, 1);
		break;
	case AUDIO_MODE_MONO:
	case AUDIO_MODE_INVALID:
	default:
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL0].gpio, 0);
		gpio_set_value(audio_sel_gpios[INDEX_AUDIO_SEL1].gpio, 0);
		break;
	}

err:
	return ret;
}
