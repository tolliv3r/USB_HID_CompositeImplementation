/*
 * main.h
 *
 * Created: 6/3/2025 3:51:16 PM
 *  Author: jackson.clary
 */ 


#ifndef MAIN_H_
#define MAIN_H_


/* -------------------------------- USB -------------------------------- */
void main_sof_action(void);

void main_suspend_action(void);
void main_resume_action(void);

void main_remotewakeup_enable(void);
void main_remotewakeup_disable(void);

/* ----------------------------- keyboard ------------------------------ */
bool main_kbd_enable(void);
void main_kbd_disable(void);

/* ----------------------------- joystick ------------------------------ */
bool main_generic_enable(void);
void main_generic_disable(void);

/* ------------------------------- LEDs -------------------------------- */
bool main_led_enable(void);
void main_led_disable(void);

void main_led_report_out(uint8_t const *report);
void main_led_report_in(uint8_t const *report);


#endif /* MAIN_H_ */