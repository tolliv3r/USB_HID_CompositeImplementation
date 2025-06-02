/*
 *  Composite HID Keyboard + Joystick configuration
 *
 *  Based on original files:
 *   – HID-Keyboard version: KeyboardImplementation/src/config/conf_usb.h
 *   – HID-Joystick version: JoystickImplementation/src/config/conf_usb.h
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"


/* ------------------------------------------------------------------------
 * ------------------------ Device-wide identifiers -----------------------
 * ----------------------------------------------------------------------*/
#define USB_DEVICE_VENDOR_ID		USB_VID_ATMEL
#define USB_DEVICE_PRODUCT_ID 		0x2133
#define  USB_DEVICE_MAJOR_VERSION	1
#define  USB_DEVICE_MINOR_VERSION	0
#define  USB_DEVICE_POWER			100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR			\
	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)


/* ------------------------------------------------------------------------
 * --------------------- USB Device string definitions --------------------
 * ----------------------------------------------------------------------*/
#define USB_DEVICE_MANUFACTURE_NAME	"ATMEL ASF"
#define USB_DEVICE_PRODUCT_NAME		"Composite HID Keyboard + Joystick"


/* ------------------------------------------------------------------------
 * ------------------------ Device speeds support -------------------------
 * ----------------------------------------------------------------------*/
#if (UC3A3||UC3A4)
#elif (SAM3XA||SAM3U)
#endif


/* ------------------------------------------------------------------------
 * ------------------- USB Device Callback definitions --------------------
 * ----------------------------------------------------------------------*/
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()				main_sof_action()
#define  UDC_SUSPEND_EVENT()			main_suspend_action()
#define  UDC_RESUME_EVENT()				main_resume_action()
#define  UDC_REMOTEWAKEUP_ENABLE()		main_remotewakeup_enable()
#define  UDC_REMOTEWAKEUP_DISABLE()		main_remotewakeup_disable()


/* ------------------------------------------------------------------------
 * -------------------  HID-KEYBOARD interface settings -------------------
 * ----------------------------------------------------------------------*/
#define UDI_HID_KBD_ENABLE_EXT()		main_kbd_enable()
#define	UDI_HID_KBD_DISABLE_EXT()		main_kbd_disable()
#define	UDI_HID_KBD_CHANGE_LED(value)	BD76319_ui_kbd_led(value)


/* ------------------------------------------------------------------------
 * -------------------  HID-JOYSTICK interface settings -------------------
 * ----------------------------------------------------------------------*/
#define  UDI_HID_GENERIC_ENABLE_EXT()	main_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()	main_generic_disable()

//! Sizes of I/O reports
#define  UDI_HID_REPORT_IN_SIZE             2	// changed from 8 -> 2
#define  UDI_HID_REPORT_OUT_SIZE            0	// changed from 8 -> 0
#define  UDI_HID_REPORT_FEATURE_SIZE        0	// changed from 4 -> 0

//! Sizes of I/O endpoints
#define  UDI_HID_GENERIC_EP_SIZE            8


/* ------------------------------------------------------------------------
 * -------------------------  HID-COMPOSITE stuff -------------------------
 * ----------------------------------------------------------------------*/
#define UDI_COMPOSITE_DESC_HS UDI_COMPOSITE_DESC_FS

#define UDI_COMPOSITE_DESC_T \
	udi_hid_kbd_desc_t udi_hid_kbd; \
	udi_hid_generic_desc_t udi_hid_generic;

#define UDI_COMPOSITE_DESC_FS \
	.udi_hid_kbd, \
	.udi_hid_generic

#define UDI_COMPOSITE_API \
	&udi_api_hid_kbd, \
	&udi_api_hid_generic

/* ------------------------------------------------------------------------
 * ------------- Bring in each UDI’s individual config header -------------
 * ----------------------------------------------------------------------*/
#include "udi_hid_kbd_conf.h"
#include "udi_hid_generic_conf.h"
#include "main.h"
#include "ui.h"          /* joystick helpers */
#include "76319_ui.h"    /* keyboard helpers */

#endif // _CONF_USB_H_