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

void BD76319_KeyToReport(uint16_t pui_KeyStatus, uint16_t pui_KeyValue);
void BD76319_ui_process(uint16_t pui_framenumber);
// void BD76319_ui_kbd_led(uint8_t value);

void kbd_ui_process(void);
void jstk_ui_process(void);

void led_ui_process(uint8_t mask);

#endif // _UI_H_