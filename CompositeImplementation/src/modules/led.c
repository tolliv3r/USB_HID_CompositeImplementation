#include "led.h"
#include <asf.h>

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

void led_setState(uint8_t mask) {
    LED_PORT.OUTSET = LED_MASK;
    LED_PORT.OUTCLR = mask;
}

void led_statusOn(void) {
    STATUS_LED_PORT.OUTCLR = LEDS_PIN;
}

void led_statusOff(void) {
    STATUS_LED_PORT.OUTSET = LEDS_PIN;
}