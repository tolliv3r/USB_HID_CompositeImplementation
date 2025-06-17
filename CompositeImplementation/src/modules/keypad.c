#include <asf.h>

#include "ui.h"
#include "led.h"
#include "keypad.h"

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

static uint8_t keyStatus2Report;
static uint8_t key2Report;

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
 * scans the keypad matrix
 */
void keypad_poll(void)
{
	// remember the previous raw row mask between scans
	static uint8_t prevRowMask = 0;
	// track the last detected key position
	uint8_t lastRow = KEYPAD_ROWS, lastCol = KEYPAD_COLS;

	// scan each column
	for (uint8_t col = 0; col < KEYPAD_COLS; ++col) {
		PORTF.OUT = kpd_colAddr[col]; // drive column select (active = low)
		if (col == 4) {
			PORTB.OUTCLR = PIN7_bm;
		} else {
			PORTB.OUTSET = PIN7_bm;
		}

		uint8_t rowBits = PORTF.IN & 0xF0;   // read raw row bits (PortF4-7)
		uint8_t rowMask = (~rowBits) & 0xF0; // invert & mask to get 1s wherever pressed

		uint8_t selectMask; // if >1 bit is set, isolate the newest bit
		if ((rowMask & (rowMask - 1)) != 0) {
			uint8_t newMask = rowMask & ~prevRowMask; // 2 or more rows r low
			if (!newMask) {
				newMask = rowMask; // shouldn't happen but for redundancy
			}
			selectMask = newMask & (uint8_t)(-newMask); // pick LSB that's set (newest)
		} else {
			selectMask = rowMask; // single key press (or none)
		}
		prevRowMask = rowMask; // save for next pass

		// decode to row index
		bool pressed = (selectMask != 0);
		uint8_t rowIndex = 0;
		switch (selectMask) {
			case 0x10: rowIndex = 0;     break;
			case 0x20: rowIndex = 1;     break;
			case 0x40: rowIndex = 2;     break;
			case 0x80: rowIndex = 3;     break;
			default:   pressed  = false; break;
		}
		if (pressed) {
			lastRow = rowIndex;
			lastCol = col;
		}
	}
	PORTB.OUTSET = PIN7_bm; // deselect all columns

	if (lastRow < KEYPAD_ROWS) { // update global press state & code
		uint8_t newCode = kpd_keyAssign[lastCol][lastRow];
		if (kpd_keyPressed == KEYPAD_RELEASED) {
			kpd_code = newCode;
			kpd_keyPressed = KEYPAD_PRESSED;
		} else if (newCode != kpd_code) {
			kpd_code = newCode;
		}
	} else {
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
		static uint8_t kpd_prevCode = 0;
		uint8_t currCode = keypad_getCode();

		if (kpd_currState == KEYPAD_PRESSED && kpd_prevState == KEYPAD_RELEASED) {
			kpd_prevCode = currCode;

			keyStatus2Report = KEYPAD_PRESSED;
			key2Report = currCode;
		} else if (kpd_currState == KEYPAD_PRESSED &&
			       currCode != kpd_prevCode) {
			kpd_prevCode = currCode;
			
			keyStatus2Report = KEYPAD_PRESSED;
			key2Report = currCode;
		} else if (kpd_currState == KEYPAD_RELEASED &&
			       kpd_prevState == KEYPAD_PRESSED) {
			keyStatus2Report = KEYPAD_RELEASED;
			key2Report = kpd_prevCode;
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


// void keypad(void) {
// 	keypad_poll();
// 	keypad_report();

// 	static bool key_was_down = false;
// 	static uint8_t active_key = 0;

// 	bool key_down = keyStatus2Report;
// 	uint8_t key_val = key2Report;

// 	if (key_down) {
// 		if (!key_was_down)
// 			active_key = key_val;
// 		else if (key_val != active_key)
// 			active_key = key_val;
// 	} else if (key_was_down) {
// 		if (active_key) {
// 			udi_hid_kbd_down(active_key);
// 			udi_hid_kbd_up(active_key);
// 		}
// 		active_key = 0;
// 	}
// 	key_was_down = key_down;
// }

void keypad(void) {
	keypad_poll();
	keypad_report();

	static bool key_was_down = false;
	static bool simult_press = false;
	static uint8_t active_key = 0;

	bool key_down = (keyStatus2Report == KEYPAD_PRESSED);
	uint8_t key_val = key2Report;

	if (key_down) {
		if (!key_was_down) {
			active_key = key_val;
			simult_press = false;
		} else if (key_val != active_key) {
			simult_press = true;
		}
	} else if (key_was_down) {
		if (!simult_press && active_key) {
			udi_hid_kbd_down(active_key);
			udi_hid_kbd_up(active_key);
		}
		active_key = 0;
		simult_press = false;
	}
	key_was_down = key_down;
}