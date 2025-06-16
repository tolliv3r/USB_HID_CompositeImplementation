/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"


/* ------------------------------------------------------------------------
 * --------------------------- Device Definition --------------------------
 * ----------------------------------------------------------------------*/
#ifndef USB_PID_ATMEL_ASF_HIDCOMPOSITE
#  define USB_PID_ATMEL_ASF_HIDCOMPOSITE  0x2133
#endif
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_HIDCOMPOSITE
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on VBUS line (mA)
#define  USB_DEVICE_ATTR                  \
		(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)


/* ------------------------------------------------------------------------
 * --------------------- USB Device string definitions --------------------
 * ----------------------------------------------------------------------*/
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "Composite HID Keyboard + Joystick"


/* ------------------------------------------------------------------------
 * ------------------------ Device speeds support -------------------------
 * ----------------------------------------------------------------------*/
#if (UC3A3||UC3A4)	// high speed
#elif (SAM3XA||SAM3U)
#endif


/* ------------------------------------------------------------------------
 * ------------------- USB Device Callback definitions --------------------
 * ----------------------------------------------------------------------*/
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
#define  UDC_REMOTEWAKEUP_ENABLE()        main_remotewakeup_enable()
#define  UDC_REMOTEWAKEUP_DISABLE()       main_remotewakeup_disable()


/* ------------------------------------------------------------------------
 * -------------------------- USB Configurations --------------------------
 * ----------------------------------------------------------------------*/
#define  USB_DEVICE_EP_CTRL_SIZE       8
#define  USB_DEVICE_NB_INTERFACE       3 // total # of interfaces
#define  USB_DEVICE_MAX_EP             4 // 0 to max endpoint


/* ------------------------------------------------------------------------
 * -------------------  HID-KEYBOARD interface settings -------------------
 * ----------------------------------------------------------------------*/
#define  UDI_HID_KBD_ENABLE_EXT()       main_kbd_enable()
#define  UDI_HID_KBD_DISABLE_EXT()      main_kbd_disable()
// #define  UDI_HID_KBD_CHANGE_LED(value)  BD76319_ui_kbd_led(value)

#define  UDI_HID_KBD_EP_IN          (1 | USB_EP_DIR_IN)
#define  UDI_HID_KBD_IFACE_NUMBER    0


/* ------------------------------------------------------------------------
 * -------------------  HID-JOYSTICK interface settings -------------------
 * ----------------------------------------------------------------------*/
#define  UDI_HID_GENERIC_ENABLE_EXT()        main_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()       main_generic_disable()

#define  UDI_HID_REPORT_IN_SIZE             2
#define  UDI_HID_REPORT_OUT_SIZE            0
#define  UDI_HID_REPORT_FEATURE_SIZE        0
#define  UDI_HID_GENERIC_EP_SIZE            8

#define  UDI_HID_GENERIC_EP_IN             (2 | USB_EP_DIR_IN)
#define  UDI_HID_GENERIC_IFACE_NUMBER       1

/* ------------------------------------------------------------------------
 * ------------------------  LED interface settings -----------------------
 * ----------------------------------------------------------------------*/
#define UDI_HID_LED_ENABLE_EXT()  main_led_enable()
#define UDI_HID_LED_DISABLE_EXT() main_led_disable()
#define UDI_HID_LED_REPORT_OUT(ptr) main_led_report_out(ptr)

#define UDI_HID_LED_REPORT_IN_SIZE       1
#define UDI_HID_LED_REPORT_OUT_SIZE      1
#define UDI_HID_LED_REPORT_FEATURE_SIZE  0
#define UDI_HID_LED_EP_SIZE              8

#define UDI_HID_LED_EP_IN               (4 | USB_EP_DIR_IN)
#define UDI_HID_LED_EP_OUT              (3 | USB_EP_DIR_OUT)
#define UDI_HID_LED_IFACE_NUMBER         2

/* ------------------------------------------------------------------------
 * -------------------------  HID-COMPOSITE stuff -------------------------
 * ----------------------------------------------------------------------*/
#define UDI_COMPOSITE_DESC_T \
		udi_hid_kbd_desc_t      udi_hid_kbd; \
		udi_hid_generic_desc_t  udi_hid_generic; \
		udi_hid_led_desc_t      udi_hid_led;

#define UDI_COMPOSITE_DESC_FS \
		.udi_hid_kbd      = UDI_HID_KBD_DESC, \
		.udi_hid_generic  = UDI_HID_GENERIC_DESC, \
		.udi_hid_led      = UDI_HID_LED_DESC

#define UDI_COMPOSITE_DESC_HS \
		.udi_hid_kbd      = UDI_HID_KBD_DESC, \
		.udi_hid_generic  = UDI_HID_GENERIC_DESC, \
		.udi_hid_led      = UDI_HID_LED_DESC

#define UDI_COMPOSITE_API \
		&udi_api_hid_kbd, \
		&udi_api_hid_generic, \
		&udi_api_hid_led


/* ------------------------------------------------------------------------
 * ------------------- USB Device Driver Configuration --------------------
 * ----------------------------------------------------------------------*/
#include "udi_hid_kbd.h"
#include "udi_hid_generic.h"
#include "udi_hid_led.h"

#include "main.h"
#include "ui.h"


#endif // _CONF_USB_H_
