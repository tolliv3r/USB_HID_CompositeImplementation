#include <asf.h>
#include "conf_usb.h"
#include <util/delay.h>

// #include "io.h"
// #include "ui.h"
// #include "led.h"
// #include "keypad.h"

#include "modules/io.h"
#include "modules/ui.h"
#include "modules/led.h"
#include "modules/keypad.h"
#include "modules/joystick.h"

static volatile bool main_b_kbd_enable = false;
static volatile bool main_b_generic_enable = false;
static volatile bool main_b_led_enable = false;

static void startupSequence(int sequence);

/* --------------------------------------------------------------------- */

int main (void)
{
	irq_initialize_vectors();  // initializes vector table
	cpu_irq_enable();          // enables CPU interrupts
	sleepmgr_init();           // initialize sleep manager
	sysclk_init();             // initialize clock

	io_init();                 // initializes board i/o pins
	keypad_init();             // initializes keypad matrix

	udc_start();               // starts USB device controller

	startupSequence(3);

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

	// if(!main_b_led_enable)
	// 	return;
	// led_ui_process();
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
	led_setStatus(report[0]);
}

/* --------------------------------------------------------------------- */

static void startupSequence(int sequence)
{
	const uint8_t seq[8] = {
		LED1_PIN,
		LED2_PIN,
		LED3_PIN,
		LED4_PIN,
		LED5_PIN,
		LED6_PIN,
		LED7_PIN,
		LED8_PIN
	};
	volatile bool user_active = false;

	switch (sequence) {
		case 1:
			/* ---------------- sequential ----------------- */
			while (!user_active) {
				for (int i = 0; i < 8 && !user_active; i++) {
					led_on(seq[i]);
					_delay_ms(850);
					led_off(seq[i]);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
				for (int i = 7; i >= 0 && !user_active; i--) {
					led_on(seq[i]);
					_delay_ms(850);
					led_off(seq[i]);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
			}
			led_allOff();
			break;
		case 2:
			/* ---------------- out-n-back ----------------- */
			while (!user_active) {
				for (int i = 0; i < 4 && !user_active; i++) {
					int left = 3 - i;
					int right = 4 + i;
					led_on(seq[left]);
					led_on(seq[right]);
					_delay_ms(900);
					led_off(seq[left]);
					led_off(seq[right]);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
				for (int i = 3; i >= 0 && !user_active; i--) {
					int left = 3 - i;
					int right = 4 + i;
					led_on(seq[left]);
					led_on(seq[right]);
					_delay_ms(900);
					led_off(seq[left]);
					led_off(seq[right]);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
			}
			led_allOff();
			break;
		case 3:
			/* --------------- grow-n-shrink ---------------- */
			while (!user_active) {
				for (int i = 0; i < 4 && !user_active; i++) {
					int left = 3 - i;
					int right = 4 + i;
					led_on(seq[left]);
					led_on(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
				for (int i = 3; i >= 0 && !user_active; i--) {
					int left = 3 - i;
					int right = 4 + i;
					led_off(seq[left]);
					led_off(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
			}
			led_allOff();
			break;
		case 4:
			/* --------------- outward waves --------------- */
			while (!user_active) {
				for (int i = 0; i < 4 && !user_active; i++) {
					int left = 3 - i;
					int right = 4 + i;
					led_on(seq[left]);
					led_on(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
				for (int i = 0; i < 4 && !user_active; i++) {
					int left = 3 - i;
					int right = 4 + i;
					led_off(seq[left]);
					led_off(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
			}
			led_allOff();
			break;
		case 5:
			/* ---------------- inward waves --------------- */
			while (!user_active) {
				for (int i = 3; i >= 0 && !user_active; i--) {
					int left = 3 - i;
					int right = 4 + i;
					led_on(seq[left]);
					led_on(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
				for (int i = 3; i >= 0 && !user_active; i--) {
					int left = 3 - i;
					int right = 4 + i;
					led_off(seq[left]);
					led_off(seq[right]);
					_delay_ms(1750);

					keypad_poll();
					if (keypad_getState() == KEYPAD_PRESSED)
						user_active = true;
					if (jstk_readMask() != 0)
						user_active = true;
				}
			}
			led_allOff();
			break;
		default:
			break;
	}
}