/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "conf_usb.h"

#include "io.h"
#include "ui.h"
#include "led.h"
#include "keypad.h"

static volatile bool main_b_kbd_enable = false;
static volatile bool main_b_generic_enable = false;

int main (void)
{
	irq_initialize_vectors();	// initializes vector table
	cpu_irq_enable();			// enables CPU interrupts
	sleepmgr_init();			// initialize sleep manager
	sysclk_init();				// initialize clock

	io_init();					// initializes board i/o pins
	led_init();					// initalizes LEDs
	keypad_init();				// initializes keypad driver

	udc_start();				// starts USB device controller

	while (true) { }
}

/* --------------------------------------------------------------------- */

void main_suspend_action(void) { }
void main_resume_action(void) { }

void main_sof_action(void) {	// called each Start of Frame event (1 ms)
	// keypad logic
	if (!main_b_kbd_enable)
	return;
	keypad_poll();
	keypad_report();
	BD76319_ui_process(udd_get_frame_number());

	// joystick logic
	if (!main_b_generic_enable)
		return;
	jstk_ui_process(udd_get_frame_number());
}

void main_remotewakeup_enable(void) { }
void main_remotewakeup_disable(void) { }

bool main_kbd_enable(void) {
	main_b_kbd_enable = true;
	return true;
}

void main_kbd_disable(void) {
	main_b_kbd_enable = false;
}

/* --------------------------------------------------------------------- */

bool main_generic_enable(void) { // joystick enable
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void) { // joystick disable
	main_b_generic_enable = false;
}