#include <asf.h>
#include <avr/io.h>
#include <conf_usb.h>
// #include <util/delay.h>

#include "76319_ui.h"
#include "keypad.h"
#include "led.h"

static volatile uint8_t kpd_keyAssign[KEYPAD_COLS][KEYPAD_ROWS];

static volatile uint8_t kpd_colAddr[KEYPAD_COLS];

static volatile uint8_t kpd_rowVal;			// <- KeyPadKeyRowValue
static volatile uint8_t kpd_colVal;			// <- KeyPadKeyColumnValue
static volatile uint8_t kpd_detectedRow;	// <- KeyPadKeyRowElement
static volatile uint8_t kpd_detectedCol;	// <- KeyPadKeyColumnElement;
static volatile uint8_t kpd_keyPressed;		// <- KeyPadKeyPressed = lastState
static volatile uint8_t kpd_code; 			// <- KeyPadKeyCode = lastCode
static volatile uint8_t kpd_prevState;		// <- lui_PrevButtonPressState (76319 main.c)
static volatile uint8_t kpd_currState;		// <- lui_ButtonPressState (76319 main.c)
static volatile uint8_t kpd_codeOut;		// <- lui_ButtonKey (76319 main.c)
static volatile uint8_t kpd_exitTestMode;
volatile uint8_t kpd_testMode;

void keypad_init(void)
{
	/*
	NULL Button		--> Column 0, Row 0	(HID_KEYPAD_9)
	CLEAR Button	--> Column 0, Row 1	(HID_KEYPAD_8)
	No Button		--> Column 0, Row 2	(0)
	No Button		--> Column 0, Row 3	(0)
	ENTER Button	--> Column 1, Row 0	(HID_KEYPAD_7)
	CANCEL Button	--> Column 1, Row 1	(HID_KEYPAD_6)
	No Button		--> Column 1, Row 2	(0)
	No Button		--> Column 1, Row 3	(0)
	Display Button	--> Column 2, Row 0	(HID_KEYPAD_5)
	No Button		--> Column 2, Row 1	(0)
	No Button		--> Column 2, Row 2	(0)
	No Button		--> Column 2, Row 3	(0)
	No Button		--> Column 3, Row 0	(0)
	No Button		--> Column 3, Row 1	(0)
	F1 Button		--> Column 3, Row 2	(HID_KEYPAD_1)
	F3 Button		--> Column 3, Row 3	(HID_KEYPAD_2)
	No Button		--> Column 4, Row 0	(0)
	No Button		--> Column 4, Row 1	(0)
	F2 Button		--> Column 4, Row 2	(HID_KEYPAD_3)
	F4 Button		--> Column 4, Row 3	(HID_KEYPAD_4)
	*/
	kpd_keyPressed = KEYPAD_RELEASED;
	kpd_currState = KEYPAD_RELEASED;
	kpd_prevState = KEYPAD_RELEASED;
	kpd_exitTestMode = 0;
	kpd_rowVal = 0;
	kpd_colVal = 0;
	kpd_code = 0;


	// kpd_keyAssign[KEYPAD_COLS][KEYPAD_ROWS]
	kpd_keyAssign[0][0] = HID_KEYPAD_9;	// NULL Button
	kpd_keyAssign[0][1] = HID_KEYPAD_8;	// CLEAR Button
	kpd_keyAssign[0][2] = 0;
	kpd_keyAssign[0][3] = 0;

	kpd_keyAssign[1][0] = HID_KEYPAD_7;	// ENTER Button
	kpd_keyAssign[1][1] = HID_KEYPAD_6;	// CANCEL Button
	kpd_keyAssign[1][2] = 0;
	kpd_keyAssign[1][3] = 0;

	kpd_keyAssign[2][0] = HID_KEYPAD_5;	// Display
	kpd_keyAssign[2][1] = 0;
	kpd_keyAssign[2][2] = 0;
	kpd_keyAssign[2][3] = 0;

	kpd_keyAssign[3][0] = 0;
	kpd_keyAssign[3][1] = 0;
	kpd_keyAssign[3][2] = HID_KEYPAD_1;	// F1 Button
	kpd_keyAssign[3][3] = HID_KEYPAD_3;	// F3 Button

	kpd_keyAssign[4][0] = 0;
	kpd_keyAssign[4][1] = 0;
	kpd_keyAssign[4][2] = HID_KEYPAD_2;	// F2 Button
	kpd_keyAssign[4][3] = HID_KEYPAD_4;	// F4 Button


	kpd_colAddr[0] = 0x0E;
	kpd_colAddr[1] = 0x0D;
	kpd_colAddr[2] = 0x0B;
	kpd_colAddr[3] = 0x07;
	kpd_colAddr[4] = 0xFF;
}


// scans the matrix, update kpd_keyPressed and kpd_code
void keypad_poll(void) {
	static volatile uint8_t kpd_count;

	kpd_detectedCol = 0;
	kpd_count = 0;

	while (kpd_count < KEYPAD_COLS)
	{
		PORTF.OUT = kpd_colAddr[kpd_count];
		kpd_colVal = kpd_colAddr[kpd_count];
		if (kpd_count == 4)
			PORTB.OUTCLR = (PIN7_bm);
		else
			PORTB.OUTSET = (PIN7_bm);

		kpd_rowVal = (PORTF.IN);
		kpd_rowVal = kpd_rowVal & 0x0f0;

		switch(kpd_rowVal)	// Convert the port row address value to a row element value for the array
		{					// kpd_rowVal is the Value read from the port (bit value)
			case 0x0E0:
				kpd_detectedRow = 0;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x0D0:
				kpd_detectedRow = 1;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x0B0:
				kpd_detectedRow = 2;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x070:
				kpd_detectedRow = 3;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			default:
				kpd_detectedRow = KEYPAD_ROWS;
				kpd_detectedCol = kpd_count;
				if (kpd_count >= KEYPAD_COLS)
					kpd_detectedCol = KEYPAD_COLS;
				kpd_count++;
				break;
		}
	}
	PORTB.OUTSET = (PIN7_bm);

	if ((kpd_detectedRow < KEYPAD_ROWS) && (kpd_detectedCol < KEYPAD_COLS))
	{
		if (kpd_keyPressed == KEYPAD_RELEASED) {
			// _delay_ms(10);
			kpd_code = kpd_keyAssign[kpd_detectedCol][kpd_detectedRow];
			kpd_keyPressed = KEYPAD_PRESSED;
		}
	}
	else
	{
		if (kpd_keyPressed == KEYPAD_PRESSED) {
			// _delay_ms(10);
			kpd_keyPressed = KEYPAD_RELEASED;
		}
	}
}

uint8_t keypad_getState(void) {
	return(kpd_keyPressed);
}
uint8_t keypad_getCode(void) {
	return(kpd_code);
}

void keypad_report(void)
{	
	kpd_testMode = PORTB.IN;
	kpd_currState = keypad_getState();
	kpd_codeOut = keypad_getCode();

	if ((kpd_testMode & 0x010) == 0) 
	{
		if (kpd_currState == KEYPAD_PRESSED && kpd_prevState == KEYPAD_RELEASED)
		{
			uint8_t kpd_testMask = 0;
			switch (kpd_codeOut) 
			{
				case HID_KEYPAD_1:	kpd_testMask = LED1_PIN;	break;	// F1
				case HID_KEYPAD_2:	kpd_testMask = LED2_PIN;	break;	// F2
				case HID_KEYPAD_3:	kpd_testMask = LED3_PIN;	break;	// F3
				case HID_KEYPAD_4:	kpd_testMask = LED4_PIN;	break;	// F4
			
				case HID_KEYPAD_5:	kpd_testMask = LED5_PIN;	break;	// Display
				case HID_KEYPAD_6:	kpd_testMask = LED6_PIN;	break;	// Cancel
				case HID_KEYPAD_7:	kpd_testMask = LED7_PIN;	break;	// Enter
				case HID_KEYPAD_8:	kpd_testMask = LED8_PIN;	break;	// Clear
				case HID_KEYPAD_9:	kpd_testMask = LED1_PIN;	break;	// Null
				default:			kpd_testMask = 0;			break;
			}
			if (kpd_testMask) led_toggle(kpd_testMask);
			kpd_exitTestMode = 1;
		}
	}
	else
	{
		if (kpd_currState == KEYPAD_PRESSED && kpd_prevState == KEYPAD_RELEASED) {
			kpd_codeOut = keypad_getCode();
			BD76319_KeyToReport(kpd_currState, kpd_codeOut);
		} else if (kpd_currState == KEYPAD_RELEASED && kpd_prevState == KEYPAD_PRESSED) {
			kpd_codeOut = keypad_getCode();
			BD76319_KeyToReport(kpd_currState, kpd_codeOut);
		}
		if (kpd_currState == KEYPAD_PRESSED && kpd_prevState == KEYPAD_RELEASED) {
			kpd_codeOut = keypad_getCode();
			// BD76319_KeyToReport(kpd_currState, kpd_codeOut);
		}
	}
	
	if (((kpd_testMode & 0x010) != 0) && (kpd_exitTestMode == 1)) {
		led_allOff();
		kpd_exitTestMode = 0;
	}
	kpd_prevState = kpd_currState;
}