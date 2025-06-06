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

// #include "76319_ui.h"

static volatile bool main_b_kbd_enable = false;
static volatile bool main_b_generic_enable = false;

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	irq_initialize_vectors();
	cpu_irq_enable();
	sleepmgr_init();			// initialize sleep manager
	sysclk_init();				// initialize clock

	udc_start();

	/* Insert application code here, after the board has been initialized. */
	while (true) { }
}

/* --------------------------------------------------------------------- */

void main_suspend_action(void) { }
void main_resume_action(void) { }

void main_sof_action(void) {
	if (!main_b_kbd_enable)
	return;
	if (!main_b_generic_enable)
		return;

	// keypad_poll();
	// keypad_report();
	
	// BD76319_ui_process(udd_get_frame_number());
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

bool main_generic_enable(void)
{
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void)
{
	main_b_generic_enable = false;
}