/*
 * udi_hid_led.c
 *
 * Created: 6/9/2025 1:49:32 PM
 *  Author: jackson.clary
 */ 
#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_hid.h"
#include "udi_hid_led.h"
#include <string.h>


bool udi_hid_led_enable(void);
void udi_hid_led_disable(void);
bool udi_hid_led_setup(void);
uint8_t udi_hid_led_getsetting(void);

// UDC_DESC_STORAGE udi_api_t udi_api_hid_led = {
// 	.enable = (bool(*)(void))udi_hid_led_enable,
// 	.disable = (void (*)(void))udi_hid_led_disable,
// 	.setup = (bool(*)(void))udi_hid_led_setup,
// 	.getsetting = (uint8_t(*)(void))udi_hid_led_getsetting,
// 	.sof_notify = NULL,
// };

UDC_DESC_STORAGE udi_api_t udi_api_hid_led = {
	.enable     = udi_hid_led_enable,
	.disable    = udi_hid_led_disable,
	.setup      = udi_hid_led_setup,
	.getsetting = udi_hid_led_getsetting,
	.sof_notify = NULL,
}

COMPILER_WORD_ALIGNED
	static uint8_t udi_hid_led_rate;

COMPILER_WORD_ALIGNED
	static uint8_t udi_hid_led_protocol;

COMPILER_WORD_ALIGNED
	static uint8_t udi_hid_led_report_out[UDI_HID_REPORT_OUT_SIZE];

// COMPILER_WORD_ALIGNED
// 	static uint8_t udi_hid_led_report_feature[UDI_HID_REPORT_FEATURE_SIZE];

UDC_DESC_STORAGE udi_hid_led_report_desc_t udi_hid_led_report_desc = { {
		0x05, 0x08,   /* usage page (LEDs)            */
		0x09, 0x00,   /* usage (undefined collection) */
		0xA1, 0x01,   /* begin collection             */
		  0x95, 0x08, /* report count (8 LEDs)        */
		  0x75, 0x01, /* report size (1 bit each)     */
		  0x19, 0x01, /* usage min                    */
		  0x29, 0x08, /* usage max                    */
		  0x91, 0x02, /* output                       */
		0xC0          /* end collection               */
	}
};


static bool udi_hid_led_setreport(void);

static void udi_hid_led_report_out_received(udd_ep_status_t status,
		iram_size_t nb_received, udd_ep_id_t ep);

static bool udi_hid_led_report_out_enable(void);

bool udi_hid_led_enable(void) {
	udi_hid_led_rate = 0;
	udi_hid_led_protocol = 0;

	if(!udi_hid_led_report_out_enable())
		return false;
	return UDI_HID_LED_ENABLE_EXT();
}

void udi_hid_led_disable(void) {
	UDI_HID_LED_DISABLE_EXT();
}

bool udi_hid_led_setup(void) {
	return udi_hid_setup(&udi_hid_led_rate,
							&udi_hid_led_protocol,
							(uint8_t *) &udi_hid_led_report_desc,
							udi_hid_led_setreport);
}

uint8_t udi_hid_led_getsetting(void) {
	return 0;
}

static bool udi_hid_led_setreport(void) {
	if ((USB_HID_REPORT_TYPE_OUTPUT == (udd_g_ctrlreq.req.wValue >> 8)) &&
		                    (0 == (0xFF & udd_g_ctrlreq.req.wValue)) &&
		 (UDI_HID_LED_REPORT_OUT_SIZE == udd_g_ctrlreq.req.wLength)) {
		udd_g_ctrlreq.payload      = (uint8_t *)udi_hid_led_report_out;
		udd_g_ctrlreq.payload_size = UDI_HID_LED_REPORT_OUT_SIZE;
		udd_g_ctrlreq.callback     = udi_hid_led_report_out_received;
		return true;
	}
	return false;
}

static void udi_hid_led_report_out_received(udd_ep_status_t status, 
	                                        iram_size_t nb_received,
	                                        udd_ep_id_t ep) {
	UNUSED(ep);
	if (status == UDD_EP_TRANSFER_OK && 
		nb_received == UDI_HID_LED_REPORT_OUT_SIZE) {
		UDI_HID_LED_REPORT_OUT(udi_hid_led_report_out);
	}
	udi_hid_led_report_out_enable();
}

static bool udi_hid_led_report_out_enable(void) {
	return udd_ep_run(UDI_HID_LED_EP_OUT,
		              false,
		              (uint8_t *)&udi_hid_led_report_out,
		              UDI_HID_LED_REPORT_OUT_SIZE,
		              udi_hid_led_report_out_received);
}