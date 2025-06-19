/*
 * GccApplication
 * File --> 76319_ui.c
 *
 */

//  ********************************************************************
/*
 * Author: Rex.Walters
 * Purpose - Project 101202 EVi Classic
 * Firmware - For P/N 76319 EVI Classic Front Panel Interface PCBA
 *				Replaces Generic_Example1 ui.c file
 *
 *----------------------------------------
 * EVI Classic Front Panel Interface Firmware Revisions
 * Revision 0
 * Created / Started March 5,2024
 * Finished (Place Date here)
 *----------------------------------------
*/
#include <asf.h>
// #include <util/delay.h>

#include "conf_usb.h"

#include "ui.h"
#include "io.h"
#include "led.h"
#include "keypad.h"
#include "joystick.h"

/* -------------------------------------- */
/* -------- UI process callbacks -------- */
/* -------------------------------------- */
void io_ui_process(void) {
	io_init();
}

void startup_ui_process(uint8_t sequence) {
	startupSequence(sequence);
}

void jstk_ui_process(void) {
	uint8_t jstk_mask = jstk_readMask();
	uint8_t jstk_testMode = PORTB.IN;

	if ((jstk_testMode & PIN4_bm) == 0) {
		if (jstk_mask) {
			led_allOff();
			led_on(jstk_mask);
		}
	} else {
		jstk_usbTask();
	}
}

void kbd_ui_process(void) {
	keypad_poll();
	keypad_report();
}

void led_ui_report(uint8_t const *mask) {
	led_setState(mask[0]);
}

void gui_ui_process(void) {
	uint16_t ledBits   = led_getMap();
	uint16_t keyBits   = kbd_getMap();
	
	uint8_t  report[4] = {
		(uint8_t)( ledBits       & 0xFF),
		(uint8_t)((ledBits >> 8) & 0xFF),
		(uint8_t)( keyBits       & 0xFF),
		(uint8_t)((keyBits >> 8) & 0xFF)
	};
	udi_hid_led_send_report_in(report);
}

void status_ui_process(void) {
	testIndicator();
}

/* -------------------------------------- */
/* ---------- helper functions ---------- */
/* -------------------------------------- */
// void delay_ms_var(uint16_t ms) {
//     while (ms--) {
//         _delay_ms(1);
//     }
// }