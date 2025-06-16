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
#include "conf_usb.h"

#include "ui.h"
#include "io.h"
#include "led.h"
#include "keypad.h"
#include "joystick.h"


void io_ui_process(void) {
	io_init();
}

void startup_ui_process(uint8_t sequence) {
	startupSequence(sequence);
}

void jstk_ui_process(void) {
	joystick();
}

void kbd_ui_process(void) {
	keypad();
}

void ui_led_report(uint8_t const *mask) {
	led_setState(mask[0]);
}

void led_ui_process(void) {
	led_usbTask();
}

void status_ui_process(void) {
	testIndicator();
}