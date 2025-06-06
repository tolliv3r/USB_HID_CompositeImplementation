/*
 * GccApplication
 * File --> 76319_ui.c
 *
 */

//  ********************************************************************
/*
 * Author: Rex.Walters
 * Purpose - Project 101202 EVi Classic
 * Firmware - For P/N 76319 EVI Classic Front Panel Interface PCBA
 *				Replaces Generic_Example1 ui.c file
 *
 *----------------------------------------
 * EVI Classic Front Panel Interface Firmware Revisions
 * Revision 0
 * Created / Started March 5,2024
 * Finished (Place Date here)
 *----------------------------------------
*/


/****************************************************/
// Section - Include File Declarations
/****************************************************/
#include <asf.h>
#include "conf_usb.h"  // Note -Conf_usb.h includes main.h
#include "ui.h"


/****************************************************/
/***************** Function Prototypes **************/
/****************************************************/
// Declaration Section - Function prototype declarations, for use only by this C file Only
uint16_t DoSomethingWithPassedValue(uint16_t pui_Value);


//  ********************************************************************
//  Definitions Section
//  ********************************************************************
// Definitions for this file only
//! Sequence process running each \c SEQUENCE_PERIOD ms
#define SEQUENCE_PERIOD 150


//  *************************************************/
// Variables Section
//  *************************************************/
// Variables for this file only

// Variable for Generic_USB class - Refer to Mircrochip example project --> GENERIC_EXAMPLE1
static uint8_t ui_KeyStatusToReport;
static uint8_t ui_KeyToReport;

// Variables for Keyboard USB Class - Refer to Mircrochip example project --> KEYBOARD_EXAMPLE1

static struct
{
	bool b_modifier;
	bool b_down;
	uint8_t u8_value;
}

Bd76319_ui_sequence[] =
{
	{false,true,false},   // Place holder for keypress code
	{false,false,false},  // Place holder for keypress code
};



//  ********************************************************************
//Section - Code C Functions
//  ********************************************************************

// These function can be used if an interrupt is used to send USB information
// Refer to the AVR example projects --> GENERIC_EXAMPLE1 or KEYBOARD_EXAMPLE1

// Debug only function
uint16_t DoSomethingWithPassedValue(uint16_t pui_Value)
{
	// Function - Just a way to debug a parameter.
	if (pui_Value == 0)
	{
	  return(0);
	}
	else if (pui_Value == 500)
	{
	  return(500);
	}
	else
	{
	  return(255);
	}
}


void BD76319_KeyToReport(uint16_t pui_KeyStatus, uint16_t pui_KeyValue)
{

	ui_KeyStatusToReport = pui_KeyStatus;
	ui_KeyToReport = pui_KeyValue;
}


// Procedure for Keyboard USB Class - Slightly modified - Refer to Mircrochip example project --> KEYBOARD_EXAMPLE1
void BD76319_ui_process(uint16_t pui_framenumber)
{
	//	   bool b_btn_state, sucess;
		   bool lui_sucess;
	static bool lui_btn_last_state = false;
	static bool lui_sequence_running = false;
	static uint8_t lui_u8_sequence_pos = 0;
			uint8_t lui_u8_value;
	static uint16_t lui_cpt_sof = 0;

	// Scan process running each 2ms

	lui_cpt_sof++;
	if ((lui_cpt_sof % 2) == 0)
	{
		return;
	}

	// Scan buttons on switch 0 to send keys sequence
	if(ui_KeyStatusToReport != lui_btn_last_state)
	{
		lui_btn_last_state = (bool) ui_KeyStatusToReport;
		lui_sequence_running = true;
	}

	// Sequence process running each period
	if (SEQUENCE_PERIOD > lui_cpt_sof)
	{
		return;
	}
	lui_cpt_sof = 0;

	if (lui_sequence_running)
	{
		// Send next key
		Bd76319_ui_sequence[lui_u8_sequence_pos].u8_value = ui_KeyToReport;
		lui_u8_value = Bd76319_ui_sequence[lui_u8_sequence_pos].u8_value;
		if (lui_u8_value!=0)
		{
			if (Bd76319_ui_sequence[lui_u8_sequence_pos].b_modifier)
			{
				if (Bd76319_ui_sequence[lui_u8_sequence_pos].b_down)
				{
					lui_sucess = udi_hid_kbd_modifier_down(lui_u8_value);
				}
				else
				{
					lui_sucess = udi_hid_kbd_modifier_up(lui_u8_value);
				}
				// ui_KeyStatusToReport = false;
				// ui_KeyToReport = false;
			}
			else
			{
				if (Bd76319_ui_sequence[lui_u8_sequence_pos].b_down)
				{
					lui_sucess = udi_hid_kbd_down(lui_u8_value);
				} else
				{
					lui_sucess = udi_hid_kbd_up(lui_u8_value);
				}
				// ui_KeyStatusToReport = false;
				// ui_KeyToReport = false;
			}
			if (!lui_sucess)
			{
				return; // Retry it on next schedule
			}
		}

		// Valid sequence position
		lui_u8_sequence_pos++;
		if (lui_u8_sequence_pos >= sizeof(Bd76319_ui_sequence) / sizeof(Bd76319_ui_sequence[0]))
		{
			lui_u8_sequence_pos = 0;
			// ui_KeyStatusToReport = false;
			// ui_KeyToReport = false;
			lui_sequence_running = false;
		}
	}
}


// Procedure for Keyboard USB Class - Slightly modified - Refer to Mircrochip example project --> KEYBOARD_EXAMPLE1
void BD76319_ui_kbd_led(uint8_t value)
{
	// Write Alarm information to Alarm LEDs
	PORTA.OUT = ~value;
}

void ui_process(uint16_t framenumber) {
    joystick();
}