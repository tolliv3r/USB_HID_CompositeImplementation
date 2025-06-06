// led.c
#include "led.h"
#include <asf.h>

#define LED_PORT	PORTA
#define LED_MASK	0xFF		// PA0–PA7

void led_init(void) {
    LED_PORT.DIRSET = LED_MASK;	// outputs
    LED_PORT.OUTSET = LED_MASK;
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