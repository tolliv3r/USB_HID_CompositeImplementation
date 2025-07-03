#ifndef KEYPAD_H
#define KEYPAD_H


#define KEYPAD_PRESSED	 1
#define KEYPAD_RELEASED	 0

#define KEYPAD_COLS		 5
#define KEYPAD_ROWS		 4

void keypad_init        (void);

uint8_t keypad_getState (void);
uint8_t keypad_getCode  (void);

void keypad_poll        (void);
void keypad_report      (void);

uint16_t kbd_getMap     (void);


#endif