#include <asf.h>

#include "ui.h"
#include "keypad.h"
#include "led.h"

// mapping of keypad layout: [column][row] → HID key code
static volatile uint8_t kpd_keyAssign[KEYPAD_COLS][KEYPAD_ROWS];

// output patterns to select each column when scanning
static volatile uint8_t kpd_colAddr[KEYPAD_COLS];

// raw scan values and detected indices
static volatile uint8_t kpd_rowVal;       // raw row input bits (PF4-PF7)
static volatile uint8_t kpd_detectedRow;  // detected row index
static volatile uint8_t kpd_detectedCol;  // detected column index

// current and previous press state and last key code
static volatile uint8_t kpd_keyPressed;   // KEYPAD_PRESSED or KEYPAD_RELEASED
static volatile uint8_t kpd_code;         // last HID code detected
static volatile uint8_t kpd_prevState;    // previous button state
static volatile uint8_t kpd_currState;    // current button state
static volatile uint8_t kpd_codeOut;      // code sent over USB

// test mode flags		
static volatile uint8_t kpd_exitTestMode; // flag to clear LEDs after test
volatile uint8_t kpd_testMode;            // hardware (switch) test mode input

/*
 * sets initial states and creates kpd_keyAssign matrix with HID codes.
 */
void keypad_init(void)
{
	/*
	NULL Button     --> Column 0, Row 0 (HID_KEYPAD_9)
	CLEAR Button    --> Column 0, Row 1 (HID_KEYPAD_8)
	No Button       --> Column 0, Row 2 (0)
	No Button       --> Column 0, Row 3 (0)
	ENTER Button    --> Column 1, Row 0 (HID_KEYPAD_7)
	CANCEL Button   --> Column 1, Row 1 (HID_KEYPAD_6)
	No Button       --> Column 1, Row 2 (0)
	No Button       --> Column 1, Row 3 (0)
	Display Button  --> Column 2, Row 0 (HID_KEYPAD_5)
	No Button       --> Column 2, Row 1 (0)
	No Button       --> Column 2, Row 2 (0)
	No Button       --> Column 2, Row 3 (0)
	No Button       --> Column 3, Row 0 (0)
	No Button       --> Column 3, Row 1 (0)
	F1 Button       --> Column 3, Row 2 (HID_KEYPAD_1)
	F3 Button       --> Column 3, Row 3 (HID_KEYPAD_2)
	No Button       --> Column 4, Row 0 (0)
	No Button       --> Column 4, Row 1 (0)
	F2 Button       --> Column 4, Row 2 (HID_KEYPAD_3)
	F4 Button       --> Column 4, Row 3 (HID_KEYPAD_4)
	*/
	kpd_keyPressed = KEYPAD_RELEASED; // no key pressed initially
	kpd_currState = KEYPAD_RELEASED;
	kpd_prevState = KEYPAD_RELEASED;
	kpd_exitTestMode = 0;
	kpd_rowVal = 0;
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

	// Column‐select output patterns (active‐low)
	kpd_colAddr[0] = 0x0E;	// 1110b – select col 0
	kpd_colAddr[1] = 0x0D;	// 1101b - select col 1
	kpd_colAddr[2] = 0x0B;	// 1011b - select col 2
	kpd_colAddr[3] = 0x07;	// 0111b - select col 3
	kpd_colAddr[4] = 0xFF;	// 1111b - select col 4
}


/*
 * scans the keypad matrix, updates kpd_keyPressed & kpd_code based on detected presses
 */
void keypad_poll(void)
{
	static volatile uint8_t kpd_count;

	// reset detection on each pass through
	kpd_detectedCol = 0;
	kpd_count = 0;

	// iterate through each column
	while (kpd_count < KEYPAD_COLS)
	{
		PORTF.OUT = kpd_colAddr[kpd_count]; // drive column lines

		// disables row driver for column 4, enabled otherwise
		if (kpd_count == 4)
			PORTB.OUTCLR = (PIN7_bm);
		else
			PORTB.OUTSET = (PIN7_bm);

		// read row inputs (PF4-PF7)
		kpd_rowVal = PORTF.IN & 0X0f0;

		// decode row bit pattern to row index
		switch(kpd_rowVal) // kpd_rowVal is the bit value read from the port
		{
			case 0x0E0: // 1110 0000b -> row 0
				kpd_detectedRow = 0;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x0D0: // 1101 0000b -> row 1
				kpd_detectedRow = 1;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x0B0: // 1011 0000b -> row 2
				kpd_detectedRow = 2;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			case 0x070: // 0111 0000b -> row 3
				kpd_detectedRow = 3;
				kpd_detectedCol = kpd_count;
				kpd_count = KEYPAD_COLS;
				break;
			default:    // no valid row
				kpd_detectedRow = KEYPAD_ROWS;
				kpd_detectedCol = kpd_count;
				if (kpd_count >= KEYPAD_COLS)
					kpd_detectedCol = KEYPAD_COLS;
				kpd_count++;
				break;
		}
	}
	PORTB.OUTSET = (PIN7_bm); // restores row driver

	// if a key was found and previously released
	if ((kpd_detectedRow < KEYPAD_ROWS) && (kpd_detectedCol < KEYPAD_COLS))
	{
		if (kpd_keyPressed == KEYPAD_RELEASED) {
			kpd_code = kpd_keyAssign[kpd_detectedCol][kpd_detectedRow];
			kpd_keyPressed = KEYPAD_PRESSED;
		}
	} else { // return to released state on release
		if (kpd_keyPressed == KEYPAD_PRESSED) {
			kpd_keyPressed = KEYPAD_RELEASED;
		}
	}
}

// get current press state
uint8_t keypad_getState(void) {
	return(kpd_keyPressed);
}
// get last detected HID code
uint8_t keypad_getCode(void) {
	return(kpd_code);
}

// toggles LED's in test mode, sends HID code over USB in normal mode
void keypad_report(void)
{	
	kpd_testMode = PORTB.IN;           // read test mode switch
	kpd_currState = keypad_getState(); // feel like this one's select explanatory
	kpd_codeOut = keypad_getCode();    // current code to be outputed

	if ((kpd_testMode & 0x010) == 0)   // test mode enabled
	{
		// on press edge, toggle corresponding LED
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
			kpd_exitTestMode = 1;	// flag for exiting test mode
		}
	}
	else // normal mode
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
		}
	}
	
	// clears LEDs when exiting test mode
	if (((kpd_testMode & 0x010) != 0) && (kpd_exitTestMode == 1)) {
		led_allOff();
		kpd_exitTestMode = 0;
	}
	// update previous state for next cycle
	kpd_prevState = kpd_currState;
}