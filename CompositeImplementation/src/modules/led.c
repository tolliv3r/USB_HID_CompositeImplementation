#include <asf.h>
#include <util/delay.h>

#include "led.h"
#include "keypad.h"
#include "joystick.h"

static bool userActivity(void);

// static volatile uint16_t sof_ms = 0;
static volatile uint8_t testMode = false;
static void led_updateState(uint8_t mask, bool state);

static bool ledMap[16] = {0};

static void delay_ms_var(uint16_t ms);

/* ---------------------------------------------------------------------- */
/* ------------------------- Regular LED Control ------------------------ */
/* ---------------------------------------------------------------------- */
void led_init(void) {
    LED_PORT.DIRSET = LED_MASK;
    LED_PORT.OUTSET = LED_MASK;

    STATUS_LED_PORT.DIRSET = LEDS_PIN;
    STATUS_LED_PORT.OUTCLR = LEDS_PIN;
}

void led_allOn(void) {			// turns all LED's on
	LED_PORT.OUTCLR = LED_MASK;

    led_updateState(LED_MASK, true);
}

void led_allOff(void) {			// turns all LED's off
	LED_PORT.OUTSET = LED_MASK;

    led_updateState(LED_MASK, false);
}

void led_on(uint8_t mask) {		// LED on
    LED_PORT.OUTCLR = mask;		// drive low (on)

    led_updateState(mask, true);
}

void led_off(uint8_t mask) {	// LED off
    LED_PORT.OUTSET = mask;		// drive high (off)

    led_updateState(mask, false);
}

void led_toggle(uint8_t mask) {	// toggle LED
    LED_PORT.OUTTGL = mask;

    for (int i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            ledMap[i] = !ledMap[i];
        }
    }
}

void led_setState(uint8_t mask) { // sets LEDs to on
    LED_PORT.OUTSET = LED_MASK;
    LED_PORT.OUTCLR = mask;

    for (int i = 0; i < 8; i++) {
        ledMap[i] = (mask & (1 << i)) != 0;
    }
}

/* ---------------------------------------------------------------------- */
/* ---------------------------- LED State Map --------------------------- */
/* ---------------------------------------------------------------------- */
static void led_updateState(uint8_t mask, bool state) {
    for (int i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            ledMap[i] = state;
        }
    }
}

uint16_t led_getMap(void) {
    uint16_t map = 0;
    for (uint8_t i = 0; i < 16; i++) {
        if (ledMap[i]) {
            map |= (1 << i);
        }
    }
    return map;
}

// bool led_getStateIndex(uint8_t index) {
//     if (index < 8)
//         return ledMap[index];
//     return false; // shouldn't happen
// }

// bool led_getStateMask(uint8_t mask) {
//     for (int i = 0; i < 8; i++) {
//         if (mask & (1 << i)) {
//             return ledMap[i];
//         }
//     }
//     return false;
// }

/* ---------------------------------------------------------------------- */
/* ----------------------------- Status LED ----------------------------- */
/* ---------------------------------------------------------------------- */
void led_statusOn(void) { // status LED on
    STATUS_LED_PORT.OUTCLR = LEDS_PIN;

    ledMap[8] = true;
}

void led_statusOff(void) { // status LED off
    STATUS_LED_PORT.OUTSET = LEDS_PIN;

    ledMap[8] = false;
}

void led_statusToggle(void) { // toggle status LED
    STATUS_LED_PORT.OUTTGL = LEDS_PIN;

    ledMap[8] = !ledMap[8];
}

/* ---------------------------------------------------------------------- */
/* --------------------------- Startup Logic ---------------------------- */
/* ---------------------------------------------------------------------- */
static bool userActivity(void) {
    testMode = PORTB.IN;
    keypad_poll();
    return ((testMode & PIN4_bm) == 0)              ||
            (keypad_getState()   == KEYPAD_PRESSED) ||
            (jstk_readMask()     != 0);
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
    static volatile bool user_active = false;

    switch (sequence) {
        case 1:
            /* ---------------- sequential ----------------- */
            while (!user_active) {
                for (int i = 0; i < 8 && !user_active; i++) {
                    led_on(seq[i]);
                    _delay_ms(850);
                    led_off(seq[i]);

                    user_active = userActivity();
                }
                for (int i = 7; i >= 0 && !user_active; i--) {
                    led_on(seq[i]);
                    _delay_ms(850);
                    led_off(seq[i]);

                    user_active = userActivity();
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
        case 6:
            /* ---------------- stripes idk ---------------- */
            while (!user_active) {
                led_allOn();
                _delay_ms(2000);
                user_active = userActivity();

                led_allOff();
                _delay_ms(2000);
                user_active = userActivity();

                led_setState(0x55);
                _delay_ms(2000);
                user_active = userActivity();

                led_setState(0xAA);
                _delay_ms(2000);
                user_active = userActivity();
            }
            led_allOff();
            break;
        case 7: {
            /* ---------------- 3 led bounce --------------- */
            uint8_t pos = 0;
            int8_t  dir = +1;

            while (!user_active) {
                uint8_t mask = (0x07 << pos);
                led_setState(mask);

                _delay_ms(1500);
                user_active = userActivity();

                pos += dir;
                if (pos == 0 || pos == (8 - 3)) {
                    dir = -dir;
                }
            }
            led_allOff();
            break;
        }
        case 8: {
            /* ---------- warp-speed acceleration ---------- */
            uint16_t delay_ms = 2500;

            while (!user_active) {
                for (uint8_t p = 0; p < 8 && !user_active; ++p) {
                    for (uint8_t i = 0; i < 8; ++i) {
                        led_setState(1 << i);
                        delay_ms_var(delay_ms);
                        
                        if (userActivity()) {
                            user_active = true;
                            break;
                        }
                    }
                    if (delay_ms > 500)
                        delay_ms -= 250;
                }

                while (!user_active) {
                    for (uint8_t i = 0; i < 8; ++i) {
                        led_setState(1 << i);
                        _delay_ms(500);
                        
                        if (userActivity()) {
                            user_active = true;
                            break;
                        }
                    }
                }
            }
            led_allOff();
            break;
        }
        default:
            break;
    }
}


void delay_ms_var(uint16_t ms) {
    while (ms--) {
        _delay_ms(1);
    }
}