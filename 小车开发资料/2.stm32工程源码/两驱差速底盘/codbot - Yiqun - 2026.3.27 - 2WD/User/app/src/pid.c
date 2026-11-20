/*
*********************************************************************************************************
*
*	模块名称 : pid控制
*	文件名称 : pid.c
*	版    本 : V1.1
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/
#include "pid.h"
#include "includes.h"

Pid_t tPidSpeedA;
Pid_t tPidSpeedB;
Pid_t tPidSpeedC;
Pid_t tPidSpeedD;
Pid_t tPidTurn;
Pid_t tPidFollow;
/*
*********************************************************************************************************
*	函 数 名: PID_Init
*	功能说明: 设置4个电机P、I、D的值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitPid(void)
{
    tPidSpeedA.P = 0.6;
    tPidSpeedA.I = 1.1;
    tPidSpeedA.D = 0.25;

    tPidSpeedA.PrevError = 0;
    tPidSpeedA.SumError = 0;


    tPidSpeedB.P = 0.6;
    tPidSpeedB.I = 1.1;
    tPidSpeedB.D = 0.25;

    tPidSpeedB.PrevError = 0;
    tPidSpeedB.SumError = 0;

    tPidSpeedC.P = 0.6;
    tPidSpeedC.I = 1.1;
    tPidSpeedC.D = 0.25;
    tPidSpeedC.PrevError = 0;
    tPidSpeedC.SumError = 0;

    tPidSpeedD.P = 0.6;
    tPidSpeedD.I = 1.1;
    tPidSpeedD.D = 0.25;
    tPidSpeedD.PrevError = 0;
    tPidSpeedD.SumError = 0;
		
		
		tPidTurn.P = 2;
    tPidTurn.I = 0;
    tPidTurn.D = 0.5;
    tPidTurn.PrevError = 0;
    tPidTurn.SumError = 0;
		
		tPidFollow.P = 30;
    tPidFollow.I = 0;
    tPidFollow.D = 8;
    tPidFollow.PrevError = 0;
    tPidFollow.SumError = 0;
}
/*
*********************************************************************************************************
*	函 数 名: wheelSpeedPidCalc
*	功能说明: 根据PID模型计算电机的pwm值
*	形    参: _ucWheelNum电机编号 _usMeasure 检测值（编码器检测的电机的速度）   _usTarget 目标值（设定的电机的速度）
*	返 回 值: 无
*********************************************************************************************************
*/

int wheelSpeedPidCalc(uint8_t _ucWheelNum, int16_t _usMeasure, int16_t _usTarget)
{
    float _dError = 0;
    float _error  = 0;
    float _tempP  = 0;
    float _tempI  = 0;
    float _tempD  = 0;
    float _tempPID = 0;

    Pid_t *_tpid;
    switch(_ucWheelNum)
    {
    case 0:
        _tpid = &tPidSpeedA;
        break;

    case 1:
        _tpid = &tPidSpeedB;
        break;

    case 2:
        _tpid = &tPidSpeedC;
        break;

    case 3:
        _tpid = &tPidSpeedD;
        break;
    }

    _error = _usTarget - _usMeasure;  /*计算当前误差*/

    _tempP = _tpid->P * _error;/*计算P项的值*/

    _tpid->SumError += _error;  /*累计误差*/
    _tempI = _tpid->I * _tpid->SumError;   /*计算I项的值*/

    if(_tempI > 2000)
    {
        _tpid->SumError = 2000 / _tpid->I;
    }
    if(_tempI < -2000)
    {
        _tpid->SumError = -2000 / _tpid->I;
    }

    _dError = _error - _tpid->PrevError;/*微分误差*/

    _tempD  = _tpid->D * _dError;/*计算D项的值*/

    _tpid->PrevError = _error ;

    _tempPID = _tempP + _tempI + _tempD;

    _tempPID = _tempPID;

    if(_tempPID > 2000)
    {
        _tempPID = 2000;
    }
    if(_tempPID < -2000)
    {
        _tempPID  = -2000;
    }

    return _tempPID;
}

int wheelTurnPidCalc(int16_t _usMeasure, int16_t _usTarget)
{
    float _dError = 0;
    float _error  = 0;
    float _tempP  = 0;
    float _tempI  = 0;
    float _tempD  = 0;
    float _tempPID = 0;

    Pid_t *_tpid;
    _tpid = &tPidTurn;


    _error = _usTarget - _usMeasure;  /*计算当前误差*/

    _tempP = _tpid->P * _error;/*计算P项的值*/

    _tpid->SumError += _error;  /*累计误差*/
    _tempI = _tpid->I * _tpid->SumError;   /*计算I项的值*/

    if(_tempI > 500)
    {
        _tpid->SumError = 500 / _tpid->I;
    }
    if(_tempI < -500)
    {
        _tpid->SumError = -500 / _tpid->I;
    }

    _dError = _error - _tpid->PrevError;/*微分误差*/

    _tempD  = _tpid->D * _dError;/*计算D项的值*/

    _tpid->PrevError = _error ;

    _tempPID = _tempP + _tempI + _tempD;

    _tempPID = _tempPID;

    if(_tempPID > 500)
    {
        _tempPID = 500;
    }
    if(_tempPID < -500)
    {
        _tempPID  = -500;
    }

    return _tempPID;
}

int wheelFollowPidCalc(float _usMeasure, float _usTarget)
{
    float _dError = 0;
    float _error  = 0;
    float _tempP  = 0;
    float _tempI  = 0;
    float _tempD  = 0;
    float _tempPID = 0;

    Pid_t *_tpid;
    _tpid = &tPidFollow;


    _error = _usTarget - _usMeasure;  /*计算当前误差*/

    _tempP = _tpid->P * _error;/*计算P项的值*/

    _tpid->SumError += _error;  /*累计误差*/
    _tempI = _tpid->I * _tpid->SumError;   /*计算I项的值*/

    if(_tempI > 500)
    {
        _tpid->SumError = 500 / _tpid->I;
    }
    if(_tempI < -500)
    {
        _tpid->SumError = -500 / _tpid->I;
    }

    _dError = _error - _tpid->PrevError;/*微分误差*/

    _tempD  = _tpid->D * _dError;/*计算D项的值*/

    _tpid->PrevError = _error ;

    _tempPID = _tempP + _tempI + _tempD;

    _tempPID = _tempPID;

    if(_tempPID > 500)
    {
        _tempPID = 500;
    }
    if(_tempPID < -500)
    {
        _tempPID  = -500;
    }

    return _tempPID;
}
/*
*********************************************************************************************************
*	函 数 名: PID_Param_SetZero
*	功能说明: 对电机的累计误差 PrevError清0
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void PID_Param_SetZero(void)
{

    tPidSpeedA.PrevError = 0;
    tPidSpeedA.SumError = 0;

    tPidSpeedB.PrevError = 0;
    tPidSpeedB.SumError = 0;

    tPidSpeedC.PrevError = 0;
    tPidSpeedC.SumError = 0;

    tPidSpeedD.PrevError = 0;
    tPidSpeedD.SumError = 0;
}