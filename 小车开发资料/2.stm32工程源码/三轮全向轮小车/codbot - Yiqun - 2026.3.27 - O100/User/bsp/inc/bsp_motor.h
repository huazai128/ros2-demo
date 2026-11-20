#ifndef __BSP_MOTOR_H__

#define __BSP_MOTOR_H__

#include "stdint.h"

void bsp_InitMotorEnGpio(void);
void MotorA_Forward(void);
void MotorA_Backward(void);
void MotorB_Forward(void);
void MotorB_Backward(void);
void MotorC_Forward(void);
void MotorC_Backward(void);
void MotorD_Forward(void);
void MotorD_Backward(void);

void MotoASetSpeed(int8_t _direction,int16_t _speed);
void MotoBSetSpeed(int8_t _direction,int16_t _speed);
void MotoCSetSpeed(int8_t _direction,int16_t _speed);
void MotoDSetSpeed(int8_t _direction,int16_t _speed);
#endif /*__MOTOR_H__*/




