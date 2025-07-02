#ifndef KEYPAD_H
#define KEYPAD_H


#define KEYPAD_PRESSED	1
#define KEYPAD_RELEASED	0

#define KEYPAD_COLS		5
#define KEYPAD_ROWS		4

void keypad_init(void);		   // initialize keypad matrix
void keypad_poll(void);		   // scan keypad matrix
void keypad_report(void);      // usb report or LED report

uint8_t keypad_getState(void); // get current key press state
uint8_t keypad_getCode(void);  // get HID code of last detected key

uint16_t kbd_getMap(void);     // gets current map of key states


#endif