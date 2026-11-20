#ifndef __FEEDBACK_H__
#define __FEEDBACK_H__

#include "bsp.h"

void MotorSpeedInfoUpdate();
void CarSpeedInfoUpdate();
void ImuInfoUpdate();
void BatteryVoltageInfoUpdate();

void IMURawDataUpdate();
void  RobotModelInfoUpdate();
#endif /*__FEEDBACK_H__*/