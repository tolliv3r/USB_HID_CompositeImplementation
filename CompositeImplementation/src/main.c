/*
 * main.c – Front-panel firmware entry point and USB event dispatcher for the EVi Classic firmware
 *
 * Author: Jackson Clary
 * Purpose: 
 *   • Initialize vector table, CPU interrupts, sleep manager, and system clock  
 *   • Configure front-panel I/O and sub-devices (LEDs, keypad, joystick)  
 *   • Start the USB device controller and run the startup LED sequence  
 *   • On USB Start-of-Frame callbacks, service keyboard, joystick, and GUI LED reports when configured  
 *   • Fallback while-loop to process keyboard, joystick, and status LED blinking w/o a USB connection
 *
 * History:
 *   Created June 3, 2025
 */

#include <asf.h>
#include "conf_usb.h"
#include <util/delay.h>

#include "modules/ui.h"

static volatile bool main_b_kbd_enable  = false;
static volatile bool main_b_jstk_enable = false;
static volatile bool main_b_led_enable  = false;

int main (void)
{
	// initializes vector table
	irq_initialize_vectors();
	// enables CPU interrupts
	cpu_irq_enable();
	// initialize sleep manager
	sleepmgr_init();
	// initialize clock
	sysclk_init();

	// initializes i/o pins & sub-devices
	io_ui_process();

	// starts USB device controller
	udc_start();

	// startup sequence (blocking)
	startup_ui_process();

	// while-loop driven operation
	// *for testing w/o a USB connection*
	while (true) {
		if (udc_is_configured()) { // usb?
			sleepmgr_enter_sleep(); // shutoff
		} else if ((PORTB.IN & PIN4_bm) == 0) {
			kbd_ui_process    ( );
			jstk_ui_process   ( );
			status_ui_process (0);
			_delay_ms         (1);
		}
	}
}


/* ------------------------------------------ */
/* ------------------- USB ------------------ */
/* ------------------------------------------ */
void main_suspend_action(void) { }
void main_resume_action(void) { }

// SoF driven operation
// *for normal use*
void main_sof_action(void) {
	if (!main_b_kbd_enable)
		return;
	kbd_ui_process   ( ); // keypad logic

	if (!main_b_jstk_enable)
		return;
	jstk_ui_process  ( ); // joystick logic

	if (!main_b_led_enable)
		return;

	gui_ui_process   ( ); // sends USB IN report
	status_ui_process(1); // status LED behavior
	idle_ui_process  ( ); // idle LED sequence
}

void main_remotewakeup_enable(void) { }
void main_remotewakeup_disable(void) { }


/* ------------------------------------------ */
/* ---------------- keyboard ---------------- */
/* ------------------------------------------ */
bool main_kbd_enable(void) {
	main_b_kbd_enable = true;
	return true;
}
void main_kbd_disable(void) {
	main_b_kbd_enable = false;
}


/* ------------------------------------------ */
/* ---------------- joystick ---------------- */
/* ------------------------------------------ */
bool main_joystick_enable(void) {
	main_b_jstk_enable = true;
	return true;
}
void main_joystick_disable(void) {
	main_b_jstk_enable = false;
}


/* ------------------------------------------ */
/* ------------------ LEDs ------------------ */
/* ------------------------------------------ */
bool main_led_enable(void) {
	main_b_led_enable = true;
	return true;
}
void main_led_disable(void) {
	main_b_led_enable = false;
}