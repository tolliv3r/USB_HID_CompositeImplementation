/*
 * led.c – Front‐panel LED control for the EVi Classic firmware
 *
 * Author: Jackson Clary
 * Purpose: Initialize and drive all front‐panel LEDs (on/off/toggle), 
 *          maintain a software LED state map, and implement startup/idle
 *          animation sequences in response to user activity.
 *
 * History:
 *   Created June 9, 2025
 */

#include <asf.h>
#include <util/delay.h>

#include "led.h"
#include "keypad.h"
#include "joystick.h"

#include <stdbool.h>


/* --------------------- local variables & prototypes ------------------- */
static void led_quiet_allOn(void);
// static void led_quiet_allOff(void);
// static void led_quiet_on(uint8_t mask);
// static void led_quiet_off(uint8_t mask);
// static void led_quiet_toggle(uint8_t mask);
static void led_quiet_setState(uint8_t mask);

static bool ledMap[16] = {0}; // map of current LED states
static void led_updateState(uint8_t mask, bool state); // update bitmap

typedef struct {
    bool     running; // idle sequence active or not
    uint8_t  stage;   // current stage of accel
    uint8_t  pass;    // # of passes through all LEDs
    uint8_t  step;    // current LED (0-7)
    uint16_t timer;   // timer between steps
    uint16_t period;  // delay for advancing steps
} idle_t;
static idle_t idle = {0};


/* ---------------------------------------------------------------------- */
/* ------------------------- regular LED control ------------------------ */
/* ---------------------------------------------------------------------- */
void led_init(void) {
    LED_PORT.DIRSET = LED_MASK;
    LED_PORT.OUTSET = LED_MASK;

    STATUS_LED_PORT.DIRSET = LEDS_PIN;
    STATUS_LED_PORT.OUTSET = LEDS_PIN;
}

void led_allOn(void) {			  // turns all LED's on
	LED_PORT.OUTCLR = LED_MASK;

    led_updateState(LED_MASK, true);
    activityEnable();
}

void led_allOff(void) {           // turns all LED's off
	LED_PORT.OUTSET = LED_MASK;

    led_updateState(LED_MASK, false);
    activityEnable();
}

void led_on(uint8_t mask) {	      // LED on
    LED_PORT.OUTCLR = mask;       // drive low (on)

    led_updateState(mask, true);
    activityEnable();
}

void led_off(uint8_t mask) {      // LED off
    LED_PORT.OUTSET = mask;       // drive high (off)

    led_updateState(mask, false);
    activityEnable();
}

void led_toggle(uint8_t mask) {   // toggle LED
    LED_PORT.OUTTGL = mask;

    for (int i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            ledMap[i] = !ledMap[i];
        }
    }
    activityEnable();
}

void led_setState(uint8_t mask) { // sets LEDs to on
    LED_PORT.OUTSET = LED_MASK;
    LED_PORT.OUTCLR = mask;

    for (int i = 0; i < 8; i++) {
        ledMap[i] = (mask & (1 << i)) != 0;
    }
    activityEnable();
}


/* ---------------------------------------------------------------------- */
/* ------------------------- silent LED control ------------------------- */
/* ---------------------------------------------------------------------- */
static void led_quiet_allOn(void) {   // turns all LED's on
    LED_PORT.OUTCLR = LED_MASK;

    led_updateState(LED_MASK, true);
}

void led_quiet_allOff(void) {         // turns all LED's off
    LED_PORT.OUTSET = LED_MASK;

    led_updateState(LED_MASK, false);
}

// static void led_quiet_on(uint8_t mask) {     // LED on
//     LED_PORT.OUTCLR = mask;     // drive low (on)

//     led_updateState(mask, true);
// }

// static void led_quiet_off(uint8_t mask) {    // LED off
//     LED_PORT.OUTSET = mask;     // drive high (off)

//     led_updateState(mask, false);
// }

// static void led_quiet_toggle(uint8_t mask) { // toggle LED
//     LED_PORT.OUTTGL = mask;

//     for (int i = 0; i < 8; i++) {
//         if (mask & (1 << i)) {
//             ledMap[i] = !ledMap[i];
//         }
//     }
// }

static void led_quiet_setState(uint8_t mask) { // sets LEDs to on
    LED_PORT.OUTSET = LED_MASK;
    LED_PORT.OUTCLR = mask;

    for (int i = 0; i < 8; i++) {
        ledMap[i] = (mask & (1 << i)) != 0;
    }
}


/* ---------------------------------------------------------------------- */
/* ------------------------- status LED control ------------------------- */
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
/* ---------------------------- LED state map --------------------------- */
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


/* ---------------------------------------------------------------------- */
/* --------------------------- startup & idle --------------------------- */
/* ---------------------------------------------------------------------- */
bool startupSequence(void) {
    led_quiet_allOn();
    led_statusOn();
    _delay_ms(15000);
    led_quiet_allOff();
    led_statusOff();
    _delay_ms(2500);

    return 0;
}

void idleStart(void) {
    idle.running   = true;
    idle.stage     = 0;
    idle.pass      = 0;
    idle.step      = 0;
    idle.timer     = 0;
    idle.period    = 250;
}
void idleStop(void) {
    idle.running = false;
    led_quiet_allOff();
}
void idlePoll(void) {
    if (!idle.running)
        return;
    if (activityCheck()) {
        idleStop();
        return;
    }

    if (++idle.timer < idle.period)
        return;
    idle.timer = 0;

    if (idle.step >= 8) {
        idle.step = 0;

        if (idle.stage == 0) {
            idle.pass++;

            if (idle.period > 100) {
                idle.period -= 75;
            } else {
                idle.period = 50;
            }
            if (idle.pass >= 4) {
                idle.stage = 1;
                idle.period = 50;
            }
        }
    }

    led_quiet_setState(1 << idle.step);
    idle.step++;
}

bool idleStatus(void) {
    return idle.running;
}