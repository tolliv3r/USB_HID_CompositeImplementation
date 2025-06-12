#include <asf.h>
#include <util/delay.h>

#include "led.h"
#include "keypad.h"
#include "joystick.h"


void led_init(void) {
    LED_PORT.DIRSET = LED_MASK;
    LED_PORT.OUTSET = LED_MASK;

    STATUS_LED_PORT.DIRSET = LEDS_PIN;
    STATUS_LED_PORT.OUTCLR = LEDS_PIN;
}

void led_allOn(void) {			// turns all LED's on
	LED_PORT.OUTCLR = LED_MASK;
}

void led_allOff(void) {			// turns all LED's off
	LED_PORT.OUTSET = LED_MASK;
}

void led_on(uint8_t mask) {		// LED on
    LED_PORT.OUTCLR = mask;		// drive low (on)
}

void led_off(uint8_t mask) {	// LED off
    LED_PORT.OUTSET = mask;		// drive high (off)
}

void led_toggle(uint8_t mask) {	// toggle LED
    LED_PORT.OUTTGL = mask;
}

void led_setState(uint8_t mask) { // sets LEDs to on
    LED_PORT.OUTSET = LED_MASK;
    LED_PORT.OUTCLR = mask;
}

void led_statusOn(void) { // status LED on
    STATUS_LED_PORT.OUTCLR = LEDS_PIN;
}

void led_statusOff(void) { // status LED off
    STATUS_LED_PORT.OUTSET = LEDS_PIN;
}

void led_statusToggle(void) { // toggle status LED
    STATUS_LED_PORT.OUTTGL = LEDS_PIN;
}

static volatile uint16_t sof_ms = 0;
void testIndicator(void) { // blink status LED when in test mode
    sof_ms++;

    if ((PORTB.IN & PIN4_bm) == 0) {
        if (sof_ms >= 500) {
            led_statusToggle();
            sof_ms = 0;
        }
    } else {
        led_statusOn();
        sof_ms = 0;
    }
}


void startupSequence(uint8_t sequence) // startup LED animation
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