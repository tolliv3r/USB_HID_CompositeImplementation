//  ********************************************************************
/*
 * Author: Rex.Walters
 * Purpose - Project 101202 EVi Classic
 * Firmware - For P/N 76319 EVI Classic Front Panel Interface PCBA
 * File 76319_ui.h
 *
 *----------------------------------------
 * 76319 Firmware Common Definitions -  Revisions
 * Revision 0
 * Created / Started March 7, 2024
 * Finished (Place Date here)
 *----------------------------------------
*/
#ifndef _UI_H_
#define _UI_H_

void kbd_ui_process(void);
void jstk_ui_process(void);


void ui_led_report(uint8_t const *mask);
void led_ui_process(void);
void status_ui_process(void);

void io_ui_process(void);

void startup_ui_process(uint8_t sequence);

void delay_ms_var(uint16_t ms);

#endif // _UI_H_