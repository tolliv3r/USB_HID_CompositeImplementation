#ifndef LED_H
#define LED_H

#include <stdint.h>

/* -------------------------------------------- */
/* ---------------- Definitions --------------- */
/* -------------------------------------------- */
#define LED1_PIN    (1u << 0)
#define LED2_PIN    (1u << 1)
#define LED3_PIN    (1u << 2)
#define LED4_PIN    (1u << 3)
#define LED5_PIN    (1u << 4)
#define LED6_PIN    (1u << 5)
#define LED7_PIN    (1u << 6)
#define LED8_PIN    (1u << 7)
#define LEDS_PIN    (1u << 6)

#define LED_MASK (\
	LED1_PIN |    \
	LED2_PIN |    \
	LED3_PIN |    \
	LED4_PIN |    \
	LED5_PIN |    \
	LED6_PIN |    \
	LED7_PIN |    \
	LED8_PIN      \
)

#define LED_PORT         PORTA
#define STATUS_LED_PORT  PORTB

/* -------------------------------------------- */
/* ---------------- Prototypes ---------------- */
/* -------------------------------------------- */
void led_init         (void);
void led_allOn        (void);
void led_allOff       (void);
void led_on           (uint8_t mask);
void led_off          (uint8_t mask);
void led_toggle       (uint8_t mask);
void led_setState     (uint8_t mask);

void led_statusOn     (void);
void led_statusOff    (void);
void led_statusToggle (void);

uint16_t led_getMap(void);
// bool led_getStateIndex(uint8_t index);
// bool led_getStateMask(uint8_t mask);

void startupSequence (uint8_t sequence);


#endif // LED_H