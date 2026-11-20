#ifndef __AUTOCONTROL_H__
#define __AUTOCONTROL_H__

#include "bsp.h"

void auto_ControlCarCmdHandle();
void auto_ControlCarStop();


void PID_SetPWM(int16_t _sMotorA, int16_t _sMotorB, int16_t _sMotorC, int16_t _sMotorD);
#endif /*__AUTOCONTROL_H__*/