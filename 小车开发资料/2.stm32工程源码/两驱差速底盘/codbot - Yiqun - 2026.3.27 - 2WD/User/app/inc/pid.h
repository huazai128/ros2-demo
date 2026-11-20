#ifndef __PID_H__
#define __PID_H__

#include "bsp.h"
typedef struct{
    float P;
    float I;
    float D;

    float PrevError;
    float SumError;

}Pid_t;

void bsp_InitPid(void);

int wheelSpeedPidCalc(uint8_t _ucWheelNum, int16_t _usMeasure, int16_t _usTarget);
void PID_Param_SetZero(void);
int wheelTurnPidCalc(int16_t _usMeasure, int16_t _usTarget);
int wheelFollowPidCalc(float _usMeasure, float _usTarget);


extern Pid_t tPidSpeedA;
extern Pid_t tPidSpeedB;
extern Pid_t tPidSpeedC;
extern Pid_t tPidSpeedD;
#endif /*__PID_H__*/