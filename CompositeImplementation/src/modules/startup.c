/*
 * startup.c
 *
 * Created: 6/11/2025 1:50:51 PM
 *  Author: jackson.clary
 */ 
#include <asf.h>
#include <util/delay.h>

#include "led.h"
#include "keypad.h"
#include "startup.h"
#include "joystick.h"


void startupSequence(int sequence)
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