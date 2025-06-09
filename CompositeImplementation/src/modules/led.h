#ifndef LED_H
#define LED_H

#include <stdint.h>

// define LED's (PORTA)
#define LED1_PIN    (1 << 0)
#define LED2_PIN    (1 << 1)
#define LED3_PIN    (1 << 2)
#define LED4_PIN    (1 << 3)
#define LED5_PIN    (1 << 4)
#define LED6_PIN    (1 << 5)
#define LED7_PIN    (1 << 6)
#define LED8_PIN    (1 << 7)

void led_init(void);
void led_allOn(void);
void led_allOff(void);
void led_on(uint8_t mask);
void led_off(uint8_t mask);
void led_toggle(uint8_t mask);

#endif