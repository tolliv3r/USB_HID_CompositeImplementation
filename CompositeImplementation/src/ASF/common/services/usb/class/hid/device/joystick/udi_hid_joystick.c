/*
 * udi_hid_joystick.c
 *
 * Created: 5/29/2025 10:23:47 AM
 *  Author: jackson.clary
 */ 

#include "conf_usb.h"
#include "usb_protocol.h"
#include "udd.h"
#include "udc.h"
#include "udi_hid.h"
#include "udi_hid_joystick.h"
#include <string.h>

/**
 * \ingroup udi_hid_generic_group
 * \defgroup udi_hid_generic_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */
bool udi_hid_joystick_enable(void);
void udi_hid_joystick_disable(void);
bool udi_hid_joystick_setup(void);
uint8_t udi_hid_joystick_getsetting(void);

//! Global structure which contains standard UDI interface for UDC
UDC_DESC_STORAGE udi_api_t udi_api_hid_joystick = {
	.enable = (bool(*)(void))udi_hid_joystick_enable,
	.disable = (void (*)(void))udi_hid_joystick_disable,
	.setup = (bool(*)(void))udi_hid_joystick_setup,
	.getsetting = (uint8_t(*)(void))udi_hid_joystick_getsetting,
	.sof_notify = NULL,
};
//@}


/**
 * \ingroup udi_hid_generic_group
 * \defgroup udi_hid_generic_group_internal Implementation of UDI HID Generic
 *
 * Class internal implementation
 * @{
 */

/**
 * \name Internal defines and variables to manage HID generic
 */
//@{

//! To store current rate of HID generic
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_joystick_rate;
//! To store current protocol of HID generic
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_joystick_protocol;
//! To signal if the report IN buffer is free (no transfer on going)
static bool udi_hid_joystick_b_report_in_free;
//! Report to send
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_joystick_report_in[UDI_HID_JSTK_REPORT_IN_SIZE];
//! Report to receive
// COMPILER_WORD_ALIGNED
// 		static uint8_t udi_hid_generic_report_out[UDI_HID_REPORT_OUT_SIZE];
//! Report to receive via SetFeature
COMPILER_WORD_ALIGNED
		static uint8_t udi_hid_joystick_report_feature[UDI_HID_JSTK_REPORT_FEATURE_SIZE];

//@}

//! HID report descriptor for HID Joystick, modified by UniWest
UDC_DESC_STORAGE udi_hid_joystick_report_desc_t udi_hid_joystick_report_desc = { { 
	0x05, 0x01,
	0x09, 0x04,
	0xA1, 0x01,
	  0x05, 0x01,
	  0x09, 0x30,
	  0x09, 0x31,
	  0x15, 0x00,
	  0x26, 0xFF, 0x00,
	  0x75, 0x08,
	  0x95, 0x02,
	  0x81, 0x02,
	0xC0
	}
};

/**
 * \name Internal routines
 */
//@{

/**
 * \brief Send a report to HID interface
 *
 */
static bool udi_hid_joystick_setreport(void);

/**
 * \brief Initialize UDD to receive setfeature data
 */
static void udi_hid_joystick_setfeature_valid(void);

/**
 * \brief Callback called when the report is received
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer is completed
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer is aborted
 * \param nb_sent    number of data received
 */
// static void udi_hid_generic_report_out_received(udd_ep_status_t status,
// 		iram_size_t nb_received, udd_ep_id_t ep);

/**
 * \brief Enable reception of out report
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
// static bool udi_hid_generic_report_out_enable(void);

/**
 * \brief Callback called when the report is sent
 *
 * \param status     UDD_EP_TRANSFER_OK, if transfer is completed
 * \param status     UDD_EP_TRANSFER_ABORT, if transfer is aborted
 * \param nb_sent    number of data transfered
 */
static void udi_hid_joystick_report_in_sent(udd_ep_status_t status,
		iram_size_t nb_sent, udd_ep_id_t ep);

//@}


//--------------------------------------------
//------ Interface for UDI HID level

bool udi_hid_joystick_enable(void)
{
	// Initialize internal values
	udi_hid_joystick_rate = 0;
	udi_hid_joystick_protocol = 0;
	udi_hid_joystick_b_report_in_free = true;
	// if (!udi_hid_generic_report_out_enable())
	// 	return false;
	return UDI_HID_JOYSTICK_ENABLE_EXT();
}


void udi_hid_joystick_disable(void)
{
	UDI_HID_JOYSTICK_DISABLE_EXT();
}


bool udi_hid_joystick_setup(void)
{
	return udi_hid_setup(&udi_hid_joystick_rate,
								&udi_hid_joystick_protocol,
								(uint8_t *) &udi_hid_joystick_report_desc,
								udi_hid_joystick_setreport);
}


uint8_t udi_hid_joystick_getsetting(void)
{
	return 0;
}


static bool udi_hid_joystick_setreport(void)
{
	if ((USB_HID_REPORT_TYPE_FEATURE == (udd_g_ctrlreq.req.wValue >> 8))
			&& (0 == (0xFF & udd_g_ctrlreq.req.wValue))
			&& (sizeof(udi_hid_joystick_report_feature) ==
					udd_g_ctrlreq.req.wLength)) {
		// Feature type on report ID 0
		udd_g_ctrlreq.payload =
				(uint8_t *) & udi_hid_joystick_report_feature;
		udd_g_ctrlreq.callback = udi_hid_joystick_setfeature_valid;
		udd_g_ctrlreq.payload_size =
				sizeof(udi_hid_joystick_report_feature);
		return true;
	}
	return false;
}

//--------------------------------------------
//------ Interface for application

bool udi_hid_joystick_send_report_in(uint8_t *data)
{
	if (!udi_hid_joystick_b_report_in_free)
		return false;
	irqflags_t flags = cpu_irq_save();
	// Fill report
	memset(&udi_hid_joystick_report_in, 0,
			sizeof(udi_hid_joystick_report_in));
	memcpy(&udi_hid_joystick_report_in, data,
	      		sizeof(udi_hid_joystick_report_in));
	udi_hid_joystick_b_report_in_free =
			!udd_ep_run(UDI_HID_JOYSTICK_EP_IN,
							false,
							(uint8_t *) & udi_hid_joystick_report_in,
							sizeof(udi_hid_joystick_report_in),
							udi_hid_joystick_report_in_sent);
	cpu_irq_restore(flags);
	return !udi_hid_joystick_b_report_in_free;

}

//--------------------------------------------
//------ Internal routines

static void udi_hid_joystick_setfeature_valid(void)
{
	if (sizeof(udi_hid_joystick_report_feature) != udd_g_ctrlreq.payload_size)
		return;	// Bad data
	// UDI_HID_GENERIC_SET_FEATURE(udi_hid_generic_report_feature);
}

// static void udi_hid_generic_report_out_received(udd_ep_status_t status,
// 		iram_size_t nb_received, udd_ep_id_t ep)
// {
// 	UNUSED(ep);
// 	if (UDD_EP_TRANSFER_OK != status)
// 		return;	// Abort reception

// 	if (sizeof(udi_hid_generic_report_out) == nb_received) {
// 		UDI_HID_GENERIC_REPORT_OUT(udi_hid_generic_report_out);
// 	}
// 	udi_hid_generic_report_out_enable();
// }


// static bool udi_hid_generic_report_out_enable(void)
// {
// 	return udd_ep_run(UDI_HID_GENERIC_EP_OUT,
// 							false,
// 							(uint8_t *) & udi_hid_generic_report_out,
// 							sizeof(udi_hid_generic_report_out),
// 							udi_hid_generic_report_out_received);
// }


static void udi_hid_joystick_report_in_sent(udd_ep_status_t status,
		iram_size_t nb_sent, udd_ep_id_t ep)
{
	UNUSED(status);
	UNUSED(nb_sent);
	UNUSED(ep);
	udi_hid_joystick_b_report_in_free = true;
}

//@}
