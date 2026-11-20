/*
*********************************************************************************************************
*
*	模块名称 : 设定舵机的占空比
*	文件名称 : bsp_servo.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/
#include "bsp.h"
#include "includes.h"
uint16_t g_feedback_SteerAngle = 0;
uint8_t g_feedback_SteerDirection = 0;

/*
*********************************************************************************************************
*	函 数 名: Servo1_SetPluse
*	功能说明: 设定舵机1的占空比
*	形    参: _pluse（占空比=_pluse/20000）
*	返 回 值: 无
*********************************************************************************************************
*/
void servoSetPluse(uint8_t _num, uint16_t _pluse)
{
    uint32_t _channel;
    TIM_HandleTypeDef _temp;
    switch(_num)
    {
    case 1:
        _temp = htimServo;
        _channel = TIM_CHANNEL_1;

        break;
    case 2:
        _temp = htimServo;
        _channel = TIM_CHANNEL_2;
        break;

    case 3:
        _temp = htimServo;
        _channel = TIM_CHANNEL_3;
        break;

    case 4:
        _temp   = htimServo;
        _channel = TIM_CHANNEL_4;
        break;
		case 5:
			  _temp   = htimServo1;
        _channel = TIM_CHANNEL_1;
        break;
		case 6:
						_temp   = htimServo1;
						_channel = TIM_CHANNEL_4;
						break;
defalut:

        break;

    }
    __HAL_TIM_SET_COMPARE(&_temp, _channel, _pluse);


    if(_pluse <= 1500)
    {
        g_feedback_SteerDirection = 0;
        g_feedback_SteerAngle = (uint16_t)((EPS_ZEROVALUE - _pluse) * EPS_RANGE) / (EPS_ZEROVALUE - EPS_LVALUE);

    }
    if(_pluse > 1500)
    {
        g_feedback_SteerDirection = 1;
        g_feedback_SteerAngle = (uint16_t)((_pluse - EPS_ZEROVALUE) * EPS_RANGE) / (EPS_RVALUE - EPS_ZEROVALUE);
    }


}



#if 1
/*
*********************************************************************************************************
*	函 数 名: Servo2_SetPluse
*	功能说明: 设定舵机3的占空比
*	形    参: _pluse（占空比=_pluse/20000）
*	返 回 值: 无
*********************************************************************************************************
*/
void Servo2_SetPluse(uint16_t _pluse)
{
    __HAL_TIM_SET_COMPARE(&htimServo, TIM_CHANNEL_2, _pluse);
}
/*
*********************************************************************************************************
*	函 数 名: Servo3_SetPluse
*	功能说明: 设定舵机3的占空比
*	形    参: _pluse（占空比=_pluse/20000）
*	返 回 值: 无
*********************************************************************************************************
*/
void Servo3_SetPluse(uint16_t _pluse)
{
    __HAL_TIM_SET_COMPARE(&htimServo, TIM_CHANNEL_3, _pluse);

}
/*
*********************************************************************************************************
*	函 数 名: Servo4_SetPluse
*	功能说明: 设定舵机4的占空比
*	形    参: _pluse（占空比=_pluse/20000）
*	返 回 值: 无
*********************************************************************************************************
*/
void Servo4_SetPluse(uint16_t _pluse)
{
    __HAL_TIM_SET_COMPARE(&htimServo, TIM_CHANNEL_4, _pluse);

}
/*
*********************************************************************************************************
*	函 数 名: Servo5_SetPluse
*	功能说明: 设定舵机5的占空比
*	形    参: _pluse（占空比=_pluse/20000）
*	返 回 值: 无
*********************************************************************************************************
*/
void Servo5_SetPluse(uint16_t _pluse)
{
    __HAL_TIM_SET_COMPARE(&htimServo1, TIM_CHANNEL_1, _pluse);
}
#endif
