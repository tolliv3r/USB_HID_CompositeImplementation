#include <asf.h>
#include "udc.h"
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
	// (for testing w/o a USB connection)
	while (true) {
		if (udc_is_configured()) {
			sleepmgr_enter_sleep();
		} else if ((PORTB.IN & PIN4_bm) == 0) {
			kbd_ui_process();
			jstk_ui_process();
			status_ui_process(0);
			_delay_ms(1);
		}
	}
}


/* ------------------------------------------ */
/* ------------------- USB ------------------ */
/* ------------------------------------------ */
void main_suspend_action(void) { }
void main_resume_action(void) { }

void main_sof_action(void) {
	if (!main_b_kbd_enable)
		return;
	kbd_ui_process   ( );

	if (!main_b_jstk_enable)
		return;
	jstk_ui_process  ( );

	if (!main_b_led_enable)
		return;

	gui_ui_process   ( );
	status_ui_process(1);
	idle_ui_process  ( );
}

void main_remotewakeup_enable(void) { }
void main_remotewakeup_disable(void) { }


/* ------------------------------------------ */
/* ---------------- Keyboard ---------------- */
/* ------------------------------------------ */
bool main_kbd_enable(void) {
	main_b_kbd_enable = true;
	return true;
}
void main_kbd_disable(void) {
	main_b_kbd_enable = false;
}


/* ------------------------------------------ */
/* ---------------- Joystick ---------------- */
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