/*
 * GccApplication
 * File --> 76319_io_initialization.c
 *
 */

//  ********************************************************************
/*
 * Author: Rex.Walters
 * Purpose - Project 101202 EVi Classic
 * Firmware - For P/N 76319 EVI Classic Front Panel Interface PCBA
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
#include "io.h"


//********************************************************************
//  Section - Code - C Functions
//********************************************************************

//===================================================================
static void initialize_PortA_io(void)
{
    // Initializes  PORT A IO bits 7-0 to Outputs.
	//              Bits are LEDs drivers to control the front panel LEDs labeled as 8 to 1.
	//              LEDs Outputs are "On" when Low, LEDs are "Off" when High.

	PORTA.DIRSET = (PIN0_bm | PIN1_bm | PIN2_bm |PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);    // Port as Output
	PORTA.OUTSET = (PIN0_bm | PIN1_bm | PIN2_bm |PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);    // Turn off outputs by setting to logic 1 (LEDs )
}


//===================================================================
static void initialize_PortB_io(void)
{
	// Initialize Horizontal Slider switches port inputs (H_Slider 9 thru 12). (See Port E for remainder of H-Slider switch Positions).
	//              Port B IO bits 3-0 to inputs with pull-ups enabled.
	// Note         Port B IO bits 5-4 are spare IO pins - Initialize to inputs with pull-ups enabled.
	// Initialize   Port B IO bit 1 drive for the Status LED. LEDs Output is "On" when Low, LEDs are "Off" when High.
	// Initialize   Port B IO bit 0 F2-F4 column output for the Keypad key-code scan signal (Front Panel Buttons).
	//              Refer to initialize_PortF_io(void) below for other keypad Key-code signals.


	// (Input Port Pins)
	PORTB.DIRCLR = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm);  // Declare pins as Inputs
	PORTB.PIN0CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups
	PORTB.PIN1CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups
	PORTB.PIN2CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups
	PORTB.PIN3CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups
	PORTB.PIN4CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups
	PORTB.PIN5CTRL = PORT_OPC_PULLUP_gc;										 // Declare pins with pull ups

	// (Output Port Pins)											             // Declare pins as Outputs
	PORTB.DIRSET = (PIN6_bm | PIN7_bm);
	PORTB.OUTSET = (PIN6_bm);													 // Set Status LED Output IO pin for LED to be "Off".
	PORTB.OUTSET = (PIN7_bm);													 // Set Output pin "F2_F4_COL" for button column to Logic High (Button Disabled).
}


//===================================================================
static void initialize_PortC_io(void)
{
	// Initializes   PORTC IO bits 1-0 as Outputs with pull-ups enabled. Reserved IO pins for other IO later such as I2C link.
	// Initializes   Vertical Slider Switches port inputs (V_Slider 1 thru 6). (See Port D for remainder of H-Slider switch Positions).
	// Initializes   PORTC IO bits 7-2 to Inputs with pull-ups enabled

	// (Input Port Pins)
	PORTC.DIRCLR = (PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);	  // Declare pins as Inputs
	PORTC.PIN2CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTC.PIN3CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTC.PIN4CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTC.PIN5CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTC.PIN6CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTC.PIN7CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups

	// (Output Port Pins)
	PORTC.DIRSET = (PIN0_bm | PIN1_bm);											  // Declare pins as Outputs
	PORTC.OUTSET = (PIN0_bm | PIN1_bm);											  // Declare pins Output levels
}

//===================================================================
static void initialize_PortD_io(void)
{
	// Initializes 6 bits of PortD IO as inputs
	// Initializes   Horizontal Slider Switches port inputs (H_Slider 1 thru 8). (See Port B for remainder of H-Slider switch Positions).
	// Initializes   PORTD IO bits 5-0 to Inputs with pull-ups enabled
	// Initializes   PORTD IO bits 7-6 to Outputs. Bits reserved for USB communication

	// (Input Port Pins)
	PORTD.DIRCLR = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm);	  // Declare pins as Inputs
	PORTD.PIN0CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTD.PIN1CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTD.PIN2CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTD.PIN3CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTD.PIN4CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTD.PIN5CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	// PORTD.PIN6CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	// PORTD.PIN7CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups

	// (Output Port Pins)
	PORTD.DIRSET = (PIN6_bm | PIN7_bm);											  // Declare pins as Outputs - Declare for USB
	PORTD.OUTCLR = (PIN6_bm | PIN7_bm);											  // Declare pins Output levels
}



//===================================================================
static void initialize_PortE_io(void)
{
	// Initializes all 8 bits of PortE IO as inputs
	// Initializes   Horizontal Slider Switches port inputs (H_Slider 1 thru 8). (See Port B for remainder of H-Slider switch Positions).
	// Initializes   PortE IO bits 7-0 to Inputs with pull-ups enabled

	// (Input Port Pins)
	PORTE.DIRCLR = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);
	PORTE.PIN0CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN1CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN2CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN3CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN4CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN5CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN6CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTE.PIN7CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
}


//===================================================================
static void initialize_PortF_io(void)
{
	// Initializes PortF IO - 4 bits are inputs and 4 bits are output for keypad key-code scanning.
	// One additional keypad key-code scan signal (F2-F4 Column) is also initialized on Port B bit 7. See Inititalize_Port_B_io() above.
	// Bits 3-0 (Keypad Column Signals) are set for output, Bits 7-4 (Keypad Row Pins) are set for input

	// (Input Port Pins)
	PORTF.DIRCLR = (PIN4_bm | PIN5_bm | PIN6_bm | PIN7_bm);						  // Keypad Row Pins
	PORTF.PIN4CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTF.PIN5CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTF.PIN6CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups
	PORTF.PIN7CTRL = PORT_OPC_PULLUP_gc;										  // Declare pins with pull ups


	// (Output Port Pins)
	PORTF.DIRSET = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm);						  // Keypad Column Pins
	PORTF.OUTSET = (PIN0_bm | PIN1_bm | PIN2_bm | PIN3_bm);						  // Set pins to Logic High (Buttons Disabled)
}


//===================================================================
void io_init(void)
{
	initialize_PortA_io();		// (Alarm LED Signals)
	initialize_PortB_io();		// (Horizontal Slider Switch signals), (Status LED Signal), (F2-F4_COL Keypad Scan Code Signal), (Spare IO)
	initialize_PortC_io();		// (Vertical Slider Switch signals), (I2C signals)
	initialize_PortD_io();		// (Vertical Slider Switch signals), (USB signals)
	initialize_PortE_io();		// (Horizontal Slider Switch signals)
	initialize_PortF_io();		// (COLUMN & ROW Keypad Scan Code signals)
}
