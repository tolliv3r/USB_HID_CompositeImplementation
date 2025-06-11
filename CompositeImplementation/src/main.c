#include <asf.h>
#include "conf_usb.h"
// #include <util/delay.h>

// #include "io.h"
// #include "ui.h"
// #include "led.h"
// #include "keypad.h"
// #include "startup.h"
// #include "joystick.h"

#include "modules/io.h"
#include "modules/ui.h"
// #include "modules/led.h"
// #include "modules/keypad.h"
#include "modules/startup.h"
// #include "modules/joystick.h"

static volatile bool main_b_kbd_enable = false;
static volatile bool main_b_generic_enable = false;
static volatile bool main_b_led_enable = false;

/* --------------------------------------------------------------------- */

int main (void)
{
	irq_initialize_vectors();  // initializes vector table
	cpu_irq_enable();          // enables CPU interrupts
	sleepmgr_init();           // initialize sleep manager
	sysclk_init();             // initialize clock

	io_init();                 // initializes board i/o pins

	udc_start();               // starts USB device controller

	startupSequence(1);        // pick preferred sequence (1-5)

	while (true) { }
}

/* --------------------------------------------------------------------- */

void main_suspend_action(void) { }
void main_resume_action(void) { }

void main_sof_action(void) {	// called each Start of Frame event (1 ms)
	if (!main_b_kbd_enable)
	return;
	kbd_ui_process();

	if (!main_b_generic_enable)
		return;
	jstk_ui_process();
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

bool main_generic_enable(void) {
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void) {
	main_b_generic_enable = false;
}

/* --------------------------------------------------------------------- */

bool main_led_enable(void) {
	main_b_led_enable = true;
	return true;
}

void main_led_disable(void) {
	main_b_led_enable = false;
}

void main_led_report_out(uint8_t const *report) {
	led_ui_process(report[0]);
}