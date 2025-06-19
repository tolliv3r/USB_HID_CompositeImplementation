#include <asf.h>
#include "conf_usb.h"

// #include "ui.h"
#include "modules/ui.h"

static volatile bool main_b_kbd_enable = false;
static volatile bool main_b_generic_enable = false;
static volatile bool main_b_led_enable = false;

// static const uint8_t startupSequence = 1;
static const uint8_t idleSequence    = 4;

int main (void)
{
	irq_initialize_vectors();  // initializes vector table
	cpu_irq_enable();          // enables CPU interrupts
	sleepmgr_init();           // initialize sleep manager
	sysclk_init();             // initialize clock

	io_ui_process(); // initializes board i/o pins

	udc_start();               // starts USB device controller

	startup_ui_process();
	idle_ui_process(idleSequence);

	while (true) { }
}

/* --------------------------------------------------------------------- */
/* -------------------------------- USB -------------------------------- */
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

	if (!main_b_led_enable)
		return;

	gui_ui_process();
	status_ui_process();
}

void main_remotewakeup_enable(void) { }
void main_remotewakeup_disable(void) { }

/* --------------------------------------------------------------------- */
/* ----------------------------- keyboard ------------------------------ */
/* --------------------------------------------------------------------- */
bool main_kbd_enable(void) {
	main_b_kbd_enable = true;
	return true;
}

void main_kbd_disable(void) {
	main_b_kbd_enable = false;
}

/* --------------------------------------------------------------------- */
/* ----------------------------- joystick ------------------------------ */
/* --------------------------------------------------------------------- */
bool main_generic_enable(void) {
	main_b_generic_enable = true;
	return true;
}

void main_generic_disable(void) {
	main_b_generic_enable = false;
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LEDs -------------------------------- */
/* --------------------------------------------------------------------- */
bool main_led_enable(void) {
	main_b_led_enable = true;
	return true;
}

void main_led_disable(void) {
	main_b_led_enable = false;
}