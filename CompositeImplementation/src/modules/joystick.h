#ifndef JOYSTICK_H
#define JOYSTICK_H


int8_t jstk_readVertIndex(void);
int8_t jstk_readHoriIndex(void);
uint8_t jstk_readMask(void);

uint8_t jstk_ledMask(int8_t percent);
uint8_t jstk_idxToAxis(int8_t idx);

void jstk_usbTask(void);	// build and send 2 byte report

uint32_t jstk_getMap(void); // 'pressed map' of both sliders


#endif // JOYSTICK_H