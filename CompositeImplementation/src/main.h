/*
 * main.h
 *
 * Created: 6/3/2025 3:51:16 PM
 *  Author: jackson.clary
 */ 

#ifndef MAIN_H_
#define MAIN_H_


/* -------------- USB -------------- */
void main_suspend_action(void);
void main_resume_action(void);

void main_sof_action(void);

void main_remotewakeup_enable(void);
void main_remotewakeup_disable(void);

/* ------------ keyboard ----------- */
bool main_kbd_enable(void);
void main_kbd_disable(void);

/* ------------ joystick ----------- */
bool main_joystick_enable(void);
void main_joystick_disable(void);

/* -------------- LEDs ------------- */
bool main_led_enable(void);
void main_led_disable(void);


#endif /* MAIN_H_ */