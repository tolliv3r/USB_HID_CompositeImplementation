#ifndef _UI_H_
#define _UI_H_


/* ---------------- IO ---------------- */
void io_ui_process(void);

/* ---------------- GUI --------------- */
void gui_ui_process(void);

/* ------------- joystick ------------- */
void jstk_ui_process(void);

/* ------------- keyboard ------------- */
void kbd_ui_process(void);

/* --------------- LEDs --------------- */
void led_ui_report(uint8_t const *mask);

/* ------------ status LED ------------ */
// void status_ui_process(void);
void status_ui_process(uint8_t usbMode);

/* ---------- startup & idle ---------- */
void startup_ui_process(void);
void idle_ui_process   (void);

/* ------ LED activity detection ------ */
void activityEnable(void);
void activityReset(void);
bool activityCheck(void);


#endif