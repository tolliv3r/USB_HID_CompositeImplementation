/*
 * keypad.c – Front‐panel keypad scanning and HID reporting for the EVi Classic firmware
 *
 * Author: Jackson Clary
 * Purpose: Scan the 5×4 keypad matrix, decode discrete key presses, manage test-mode LED toggling,
 *          and send USB HID keyboard reports in normal mode or toggle LEDs in hardware test mode.
 *
 * History:
 *   Created May 22, 2025
 */

#include <asf.h>

#include "ui.h"
#include "led.h"
#include "keypad.h"


// mapping of keypad layout: [column][row] → HID key code
static volatile uint8_t kpd_keyAssign[KEYPAD_COLS][KEYPAD_ROWS];

// output patterns to select each column when scanning
static volatile uint8_t kpd_colAddr[KEYPAD_COLS];

// raw scan values and detected indices
static volatile uint8_t kpd_rowVal;             // raw row input bits (PF4-PF7)
static volatile uint8_t kpd_detectedRow;        // detected row index
static volatile uint8_t kpd_detectedCol;        // detected column index

// current and previous press state and last key code
static volatile uint8_t kpd_keyPressed;         // KEYPAD_PRESSED or KEYPAD_RELEASED
static volatile uint8_t kpd_code;               // last HID code detected
static volatile uint8_t kpd_prevState;          // previous button state
static volatile uint8_t kpd_currState;          // current button state
static volatile uint8_t kpd_currentCode;        // code sent over USB
static volatile bool    kpd_multiPress = false; // multipress detection

// test mode flags		
static volatile uint8_t kpd_exitTestMode;       // flag to clear LEDs after test
static volatile uint8_t kpd_testMode;           // hardware (switch) test mode input

// key map variables
static bool keyMap[16] = {0};                   // map of current keypad state
static const int8_t keyIndex[KEYPAD_COLS][KEYPAD_ROWS] = {
	{  8,  7, -1, -1 },
	{  6,  5, -1, -1 },
	{  4, -1, -1, -1 },
	{ -1, -1,  0,  2 },
	{ -1, -1,  1,  3 },
};

/*
 * sets initial states and creates kpd_keyAssign matrix with HID codes.
 */
void keypad_init(void)
{
	/*
	NULL Button     --> Column 0, Row 0 (HID_N)
	CLEAR Button    --> Column 0, Row 1 (HID_BACKSPACE)
	No Button       --> Column 0, Row 2 (0)
	No Button       --> Column 0, Row 3 (0)
	ENTER Button    --> Column 1, Row 0 (HID_ENTER)
	CANCEL Button   --> Column 1, Row 1 (HID_ESCAPE)
	No Button       --> Column 1, Row 2 (0)
	No Button       --> Column 1, Row 3 (0)
	Display Button  --> Column 2, Row 0 (HID_D)
	No Button       --> Column 2, Row 1 (0)
	No Button       --> Column 2, Row 2 (0)
	No Button       --> Column 2, Row 3 (0)
	No Button       --> Column 3, Row 0 (0)
	No Button       --> Column 3, Row 1 (0)
	F1 Button       --> Column 3, Row 2 (HID_F1)
	F3 Button       --> Column 3, Row 3 (HID_F2)
	No Button       --> Column 4, Row 0 (0)
	No Button       --> Column 4, Row 1 (0)
	F2 Button       --> Column 4, Row 2 (HID_F3)
	F4 Button       --> Column 4, Row 3 (HID_F4)
	*/
	kpd_keyPressed = KEYPAD_RELEASED; // no key pressed initially
	kpd_currState = KEYPAD_RELEASED;
	kpd_prevState = KEYPAD_RELEASED;
	kpd_exitTestMode = 0;
	kpd_rowVal = 0;
	kpd_code = 0;


	// kpd_keyAssign[KEYPAD_COLS][KEYPAD_ROWS]
	kpd_keyAssign[0][0] = HID_N;         // NULL Button
	kpd_keyAssign[0][1] = HID_BACKSPACE; // CLEAR Button
	kpd_keyAssign[0][2] = 0;
	kpd_keyAssign[0][3] = 0;

	kpd_keyAssign[1][0] = HID_ENTER;     // ENTER Button
	kpd_keyAssign[1][1] = HID_ESCAPE;    // CANCEL Button
	kpd_keyAssign[1][2] = 0;
	kpd_keyAssign[1][3] = 0;

	kpd_keyAssign[2][0] = HID_D;         // Display
	kpd_keyAssign[2][1] = 0;
	kpd_keyAssign[2][2] = 0;
	kpd_keyAssign[2][3] = 0;

	kpd_keyAssign[3][0] = 0;
	kpd_keyAssign[3][1] = 0;
	kpd_keyAssign[3][2] = HID_F1;        // F1 Button
	kpd_keyAssign[3][3] = HID_F3;        // F3 Button

	kpd_keyAssign[4][0] = 0;
	kpd_keyAssign[4][1] = 0;
	kpd_keyAssign[4][2] = HID_F2;        // F2 Button
	kpd_keyAssign[4][3] = HID_F4;        // F4 Button

	// Column‐select output patterns (active‐low)
	kpd_colAddr[0] = 0x0E;	// 1110b – select col 0
	kpd_colAddr[1] = 0x0D;	// 1101b - select col 1
	kpd_colAddr[2] = 0x0B;	// 1011b - select col 2
	kpd_colAddr[3] = 0x07;	// 0111b - select col 3
	kpd_colAddr[4] = 0xFF;	// 1111b - select col 4
}


// get current press state
uint8_t keypad_getState(void) {
	return(kpd_keyPressed);
}
// get last detected HID code
uint8_t keypad_getCode(void) {
	return(kpd_code);
}


/*
 * scans the keypad matrix
 */
void keypad_poll(void)
{
	for (int i = 0; i < 9; i++) {
		keyMap[i] = 0;
	}

	// remember the previous raw row mask between scans
	static uint8_t prevRowMask = 0;
	// track the last detected key position
	uint8_t lastRow = KEYPAD_ROWS, lastCol = KEYPAD_COLS;
	// total keys seen this scan
	uint8_t pressedCount = 0;

	// scan each column
	for (uint8_t col = 0; col < KEYPAD_COLS; ++col) {
		PORTF.OUT = kpd_colAddr[col]; // drive column select (active = low)
		// column 4 is wired to PB7 (not PF0-PF3), so drive PB7 low only when scanning that column
		if (col == 4) {
			PORTB.OUTCLR = PIN7_bm;
		} else {
			PORTB.OUTSET = PIN7_bm;
		}

		uint8_t rowBits = PORTF.IN & 0xF0;   // read raw row bits (PortF4-7)
		uint8_t rowMask = (~rowBits) & 0xF0; // invert & mask to get 1s wherever pressed

		// add up the bits in column
		if (rowMask & 0x10) pressedCount++;
		if (rowMask & 0x20) pressedCount++;
		if (rowMask & 0x40) pressedCount++;
		if (rowMask & 0x80) pressedCount++;

		for (uint8_t bit = 0; bit < KEYPAD_ROWS; bit++) {
			if (rowMask & (1 << (bit + 4))) {
				int8_t idx = keyIndex[col][bit];
				if (idx >= 0 && idx < 9) {
					keyMap[idx] = 1;
				}
			}
		}

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
		// only update kpd_code on an *event* (initial press or key-change)
		if (kpd_keyPressed == KEYPAD_RELEASED) {	// skip if same key is still held
			kpd_code = newCode;						// first key-down edge
			kpd_keyPressed = KEYPAD_PRESSED;
		} else if (newCode != kpd_code) {			// new keypress w/o releasing old
			kpd_code = newCode;						// update internal currently pressed code
		}
	} else {
		if (kpd_keyPressed == KEYPAD_PRESSED) {
			kpd_keyPressed = KEYPAD_RELEASED;
		}
	}
	kpd_multiPress = (pressedCount > 1);
}

// toggles LED's in test mode, sends HID code over USB in normal mode
void keypad_report(void)
{	
	kpd_testMode = PORTB.IN;           // read test mode switch
	kpd_currState = keypad_getState(); // feel like this one's select explanatory
	kpd_currentCode = keypad_getCode();    // current code to be outputed

	if ((kpd_testMode & 0x010) == 0)   // test mode enabled
	{
		// on press edge, toggle corresponding LED
		if (kpd_currState == KEYPAD_PRESSED && kpd_prevState == KEYPAD_RELEASED)
		{
			uint8_t kpd_testMask = 0;
			switch (kpd_currentCode) 
			{
				case HID_F1:        kpd_testMask = LED1_PIN;  break;  // F1
				case HID_F2:        kpd_testMask = LED2_PIN;  break;  // F2
				case HID_F3:        kpd_testMask = LED3_PIN;  break;  // F3
				case HID_F4:        kpd_testMask = LED4_PIN;  break;  // F4
				case HID_D:         kpd_testMask = LED5_PIN;  break;  // Display
				case HID_ESCAPE:    kpd_testMask = LED6_PIN;  break;  // Cancel
				case HID_ENTER:     kpd_testMask = LED7_PIN;  break;  // Enter
				case HID_BACKSPACE:	kpd_testMask = LED8_PIN;  break;  // Clear
				case HID_N:         kpd_testMask = LED1_PIN;  break;  // Null
				default:            kpd_testMask = 0;         break;
			}
			if (kpd_testMask) led_toggle(kpd_testMask);

			kpd_exitTestMode = 1;	// flag for exiting test mode
		}
	}
	else // normal mode
	{
		bool kpd_anyPressed = (keypad_getState() == KEYPAD_PRESSED);

		static bool    kpd_firstKey  = false;
		static uint8_t kpd_firstCode = 0;
		static bool    kpd_block     = false;

		if (!kpd_firstKey) {
			if (kpd_anyPressed) {
				kpd_firstKey = true;
				kpd_firstCode = kpd_currentCode;
				kpd_block = false;
			}
		} else {
			if (kpd_anyPressed && !kpd_block && kpd_multiPress) {
				kpd_block = true;
			}
			if (!kpd_anyPressed) {
				if (!kpd_block) {
					udi_hid_kbd_down(kpd_firstCode);
					udi_hid_kbd_up(kpd_firstCode);
				}
				kpd_firstKey = false;
				kpd_block = false;
			}
		}
	}

	if (((kpd_testMode & 0x010) != 0) && (kpd_exitTestMode == 1)) {
		led_quiet_allOff();
		kpd_exitTestMode = 0;
	}
	kpd_prevState = kpd_currState;
}


// get current map of keypad states
uint16_t kbd_getMap(void) {
	uint16_t bits = 0;

	for (uint8_t i = 0; i < 9; ++i) {
		if (keyMap[i]) {
			bits |= (1 << i);
		}
	}

	return bits;
}