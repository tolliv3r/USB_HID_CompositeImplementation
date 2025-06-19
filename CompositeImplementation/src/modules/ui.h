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


/* ----------------- IO ----------------- */
void io_ui_process(void);

/* --------------- Startup -------------- */
void startup_ui_process(uint8_t startupSeq);
void idle_ui_process   (uint8_t idleSeq);

/* -------------- Joystick -------------- */
void jstk_ui_process(void);

/* -------------- Keyboard -------------- */
void kbd_ui_process(void);

/* ---------------- LEDs ---------------- */
void led_ui_report(uint8_t const *mask);

/* ----------------- GUI ---------------- */
void gui_ui_process(void);

/* ------------- Status LED ------------- */
void status_ui_process(void);


#endif // _UI_H_