/*
 * main.h
 *
 * Created: 6/3/2025 3:51:16 PM
 *  Author: jackson.clary
 */ 


#ifndef MAIN_H_
#define MAIN_H_


//	\brief called by HID interface
//	callback running when USB Host enables keyboard interface
bool main_kbd_enable(void);	// returns TRUE if keyboard startup is ok

//	\brief called by HID interface
//	callback running when USB Host disables keyboard interface
void main_kbd_disable(void);

//	\brief called when a start of frame is received on USB line
void main_sof_action(void);

//	\brief enters the application in low power mode
//	callback called when USB host sets USB line in suspend state
void main_suspend_action(void);

//	\brief called by UDD when the USB line exit of suspend state
void main_resume_action(void);

//	\brief called by UDC when USB Host requests to enable remote wakeup
void main_remotewakeup_enable(void);

//	\brief called by UDC when USB Host requests to disable remote wakeup
void main_remotewakeup_disable(void);

/* --------------------------------------------------------------------- */

bool main_generic_enable(void);
void main_generic_disable(void);


#endif /* MAIN_H_ */