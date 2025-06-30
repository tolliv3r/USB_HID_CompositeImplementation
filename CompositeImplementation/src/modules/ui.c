/*
 * ui.c – Central UI controller for the EVi Classic firmware
 *
 * Author: Rex Walters
 * Purpose: Initialize and coordinate all front-panel I/O and subsystems, including
 *          GPIO setup (via io_init), composite USB HID report assembly for LEDs,
 *          keypad, and joystick, processing of host LED commands, status LED control,
 *          startup and idle animation sequences, and user activity tracking.
 *
 * History:
 *   Created March 5, 2024
 *   Full overhaul May 22, 2025
 */

#include <asf.h>
#include "conf_usb.h"

#include "ui.h"
#include "io.h"
#include "led.h"
#include "keypad.h"
#include "joystick.h"

#define IDLE (1 << 1)

#define STOP       0x42
#define START      0x45
#define STATUS_ON  0x48
#define STATUS_OFF 0x51

static volatile uint16_t sof_ms       = 0;
static volatile bool     startupCheck = 1;
static bool              userActive   = 0;

static volatile uint8_t jstk_exitTestMode;
static volatile uint8_t jstk_testMode;

/* --------------------------------------- */
/* ------------------ IO ----------------- */
/* --------------------------------------- */
void io_ui_process(void) {
	io_init();
}
// initializes all IO ports


/* --------------------------------------- */
/* ----------------- GUI ----------------- */
/* --------------------------------------- */
void gui_ui_process(void) {
	uint16_t ledBits   = led_getMap ();
	uint16_t keyBits   = kbd_getMap ();
	uint32_t joyBits   = jstk_getMap();
	
	uint8_t  report[7] = {
		(uint8_t)( ledBits        & 0xFF),
		(uint8_t)((ledBits >> 8)  & 0xFF) |
		          (idleStatus()   ? IDLE:0),
		(uint8_t)( keyBits        & 0xFF),
		(uint8_t)((keyBits >> 8)  & 0xFF),
		(uint8_t)( joyBits        & 0xFF),
		(uint8_t)((joyBits >> 8)  & 0xFF),
		(uint8_t)((joyBits >> 16) & 0xFF),
	};
	udi_hid_led_send_report_in(report);
}
// 7 byte output for GUI


/* ---------------------------------------- */
/* --------------- Keyboard --------------- */
/* ---------------------------------------- */
void kbd_ui_process(void) {
	keypad_poll();
	keypad_report();
}
// keyboard logic


/* ---------------------------------------- */
/* --------------- Joystick --------------- */
/* ---------------------------------------- */
void jstk_ui_process(void) {
	uint8_t jstk_mask = jstk_readMask();
	uint8_t jstk_testMode = PORTB.IN;

	if ((jstk_testMode & PIN4_bm) == 0) {
		if (jstk_mask) {
			led_allOff();
			led_on(jstk_mask);

			jstk_exitTestMode = 1;
			// activityEnable();
		}
	} else {
		jstk_usbTask();

		if (((jstk_testMode & 0x010) != 0) && 
			 (jstk_exitTestMode      == 1)) {
			led_quiet_allOff();
			jstk_exitTestMode = 0;
		}
	}
}
// joystick logic


/* ---------------------------------------- */
/* ----------------- LEDs ----------------- */
/* ---------------------------------------- */
void led_ui_report(uint8_t const *code) {
	uint8_t ledMask = code[0];
	uint8_t command = code[1];

	if        (command == STOP)       {
		activityEnable();
	} else if (command == START)      {
		activityReset();
		idle_start();
	} else if (command == STATUS_ON)  {
		led_statusOn();
	} else if (command == STATUS_OFF) {
		led_statusOff();
	} else                            {
		led_setState(ledMask);
	}
}
// allows host PC to manually control LEDs


/* ---------------------------------------- */
/* -------------- Status LED -------------- */
/* ---------------------------------------- */
void status_ui_process(void) {
	static bool prev = false;
	bool curr = ((PORTB.IN & PIN4_bm) == 0);
	sof_ms++;

	if (curr) { // if currently in test mode
		if (sof_ms >= 500) {
			led_statusToggle();
			sof_ms = 0;
		}
	} else {
		if (prev) // if just exiting TM
			led_statusOff();
		sof_ms = 0; // reset counter
	}
	prev = curr;
}
// blink status LED in test mode


/* ---------------------------------------- */
/* ------------ Startup & Idle ------------ */
/* ---------------------------------------- */
void startup_ui_process(void) {
	startupCheck = startupSequence();
}
// performs startup LED sequence


void idle_ui_process(void) {
	idle_poll();
}
// performs idle LED sequence


/* ---------------------------------------- */
/* -------- LED Activity Detection -------- */
/* ---------------------------------------- */
void activityEnable(void) {
	userActive = 1;
}

void activityReset(void) {
	userActive = 0;
}

bool activityCheck(void) {
	return userActive;
}