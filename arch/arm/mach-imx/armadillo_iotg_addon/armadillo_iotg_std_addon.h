// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2024 Atmark Techno, Inc. All Rights Reserved.
 */

#ifndef __ASM_ARCH_ARMADILLO_IOTG_STD_ADD_ON_H__
#define __ASM_ARCH_ARMADILLO_IOTG_STD_ADD_ON_H__

struct addon_device_descriptor {
	__be16	vendor_id;
	__be16	product_id;
	__be16	revision;
	__be32	serial_no;
	__u8	reserved[22];
	__u8	vendor_specific[96];
} __attribute__ ((packed));

struct addon_device_identifier {
	const char *vendor_name;
	const char *product_name;
};

enum addon_interface {
	ADDON_INTERFACE1 = 1,
	ADDON_INTERFACE2 = 2,
	NR_ADDON_INTERFACES,
};

enum addon_board_type {
	ADDON_BOARD_TYPE_IOTG,
	ADDON_BOARD_TYPE_X1,
};

#define NR_ADDON_PINS	(60)
struct addon_device {
	enum addon_interface intf;
	struct addon_device_descriptor desc;
	struct addon_device_identifier ident;
	int gpios[NR_ADDON_PINS];
};

struct armadillo_iotg_addon {
	struct device *dev;
	struct i2c_adapter *adap;
	enum addon_board_type type;
	int power_gpio;
	struct addon_device adev[NR_ADDON_INTERFACES];
};

#define to_addon(_adev) container_of(_adev, struct armadillo_iotg_addon, adev[_adev->intf])

/* EEPROM vendor ID */
#define ADDON_VENDOR_ID_ATMARK_TECHNO	(0x0001)
#define ADDON_VENDOR_ID_SATORI		(0x0005)
#define ADDON_VENDOR_ID_ADVALY		(0x0007)

/* EEPROM Atmark Techno product ID */
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_WI_SUN	(0x0001)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_EN_OCEAN	(0x0002)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_SERIAL	(0x0003)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_DIDOAD	(0x0004)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RN4020	(0x0005)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_CAN	(0x0006)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_ZIGBEE	(0x0007)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RS232C	(0x0008)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_RS485	(0x0009)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_SD	(0x000a)
#define ADDON_PRODUCT_ID_ATMARK_TECHNO_THREAD	(0x000c)

/* EEPROM Satori product ID */
#define ADDON_PRODUCT_ID_SATORI_B_ROUTE		(0x0001)
#define ADDON_PRODUCT_ID_SATORI_920M		(0x0002)
#define ADDON_PRODUCT_ID_SATORI_LOW_POWER	(0x0003)

/* EEPROM Advaly product ID */
#define ADDON_PRODUCT_ID_ADVALY_USBLAN		(0x0002)
#define ADDON_PRODUCT_ID_ADVALY_SPICAN		(0x0003)
#define ADDON_PRODUCT_ID_ADVALY_DIO		(0x0004)
#define ADDON_PRODUCT_ID_ADVALY_AUDIO_DIO	(0x0006)

#define dtb_begin(f)	__dtb_##f##_begin
#define dtb_end(f)	__dtb_##f##_end
#define dtb_size(f)	(dtb_end(f) - dtb_begin(f))

#define extern_dtb(f)		\
extern uint8_t dtb_begin(f)[];	\
extern uint8_t dtb_end(f)[];

int armadillo_iotg_addon_dt_overlay(struct device *dev, void *begin, size_t size);


int addon_setup_atmark_techno_wi_sun(struct addon_device *adev);
int addon_setup_atmark_techno_en_ocean(struct addon_device *adev);
int addon_setup_atmark_techno_serial(struct addon_device *adev);
int addon_setup_atmark_techno_didoad(struct addon_device *adev);
int addon_setup_atmark_techno_rn4020(struct addon_device *adev);
int addon_setup_atmark_techno_rs232c(struct addon_device *adev);
int addon_setup_atmark_techno_rs485(struct addon_device *adev);
int addon_setup_atmark_techno_sd(struct addon_device *adev);
int addon_setup_atmark_techno_thread(struct addon_device *adev);

int addon_setup_satori_wireless(struct addon_device *adev);

int addon_setup_advaly_usblan(struct addon_device *adev);
int addon_setup_advaly_spican(struct addon_device *adev);
int addon_setup_advaly_dio(struct addon_device *adev);
int addon_setup_advaly_audio_dio(struct addon_device *adev);

#endif
