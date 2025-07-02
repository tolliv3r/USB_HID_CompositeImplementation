#ifndef JOYSTICK_H
#define JOYSTICK_H


int8_t jstk_readVertIndex (void);
int8_t jstk_readHoriIndex (void);
uint8_t jstk_idxToAxis    (int8_t idx);

// test mode LED map
uint8_t jstk_readMask     (void);
uint8_t jstk_ledMask      (int8_t idx);

// build and send 2 byte report
void jstk_usbTask         (void);

// 'pressed map' of both sliders
uint32_t jstk_getMap      (void);


#endif