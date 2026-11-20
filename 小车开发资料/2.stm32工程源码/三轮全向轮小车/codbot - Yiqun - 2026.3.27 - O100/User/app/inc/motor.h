#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "bsp.h"



#define PS2_SPEEDPULSE_ZERO 1000
#define PS2_SPEEDPULSE_MAX 1600
#define AUTO_SPEED_MAX 600
#define PS2_SPEED_MAX 500
#define TIRE_SPACE 200//¬÷Ã•øÌ∂»∑ΩœÚæ‡¿Î
#define WHEELBASE 185//÷·æ‡

extern int16_t _sMotorA_Speed_M;
extern int16_t _sMotorB_Speed_M;
extern int16_t _sMotorC_Speed_M;
extern int16_t _sMotorD_Speed_M;
void MotorControl(uint8_t _ucAngularDirection,uint16_t _ucAngularSpeed,uint8_t _ucMotionDirection,uint16_t _usMotionSpeed,uint8_t _ucMotionDirection_Y,uint16_t _usMotionSpeed_Y);
void MotorControl_Ps2(uint8_t _Ps2PadXValue,uint8_t  _Ps2PadYValue);
//void MotorControl_Auto(uint8_t _SteerDirection,uint8_t _SteerAngle,uint8_t _MotionDirection,uint16_t _MotionSpeed);
void MotorControl_Auto(uint8_t _angular_Direction,uint8_t _angular_speed,uint8_t _MotionDirection,uint16_t _MotionSpeed);
void ble_motorControl(uint8_t _angular_Direction,uint8_t _angular_speed,uint8_t _MotionDirection,uint16_t _MotionSpeed);
#endif /*__MOTOR_H__*/