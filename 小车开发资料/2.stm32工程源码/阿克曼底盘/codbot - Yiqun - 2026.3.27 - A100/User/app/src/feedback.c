/*
*********************************************************************************************************
*
*	模块名称 : 数据反馈处理
*	文件名称 : feedback.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/
#include "includes.h"



long  iaAccelData[3] = {0};
short saGyroData[3] = {0};

float QuatW = 0, QuatX = 0, QuatY = 0, QuatZ = 0;
short AX = 0, AY = 0, AZ = 0;
short gx = 0, gy = 0, gz = 0;

extern uint16_t g_feedback_SteerAngle;
extern uint8_t g_feedback_SteerDirection;


/*
*********************************************************************************************************
*	函 数 名: MotorSpeed_Feedback
*	功能说明: 电机转速反馈（报头:0xFD 报文长度:0x09 功能位:0x03）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorSpeedInfoUpdate()
{
    uint8_t _MotorSpeed_data[9] = {0} ;
    uint8_t _MotorSpeed_buf[12] = {0} ;


    _MotorSpeed_data[0] = 0x03;
    _MotorSpeed_data[1] = (g_WheelSpeed[0] >> 8) & 0xFF;
    _MotorSpeed_data[2] = g_WheelSpeed[0] & 0xFF;
    _MotorSpeed_data[3] = (g_WheelSpeed[1] >> 8) & 0xFF;
    _MotorSpeed_data[4] = g_WheelSpeed[1] & 0xFF;
    _MotorSpeed_data[5] = (g_WheelSpeed[2] >> 8) & 0xFF;
    _MotorSpeed_data[6] = g_WheelSpeed[2] & 0xFF;
    _MotorSpeed_data[7] = (g_WheelSpeed[3] >> 8) & 0xFF;
    _MotorSpeed_data[8] = g_WheelSpeed[3] & 0xFF;

    _MotorSpeed_buf[0]  = 0xFD;
    _MotorSpeed_buf[1]  = 0x09;
    _MotorSpeed_buf[2]  = _MotorSpeed_data[0];
    _MotorSpeed_buf[3]  = _MotorSpeed_data[1];
    _MotorSpeed_buf[4]  = _MotorSpeed_data[2];
    _MotorSpeed_buf[5]  = _MotorSpeed_data[3];
    _MotorSpeed_buf[6]  = _MotorSpeed_data[4];
    _MotorSpeed_buf[7]  = _MotorSpeed_data[5];
    _MotorSpeed_buf[8]  = _MotorSpeed_data[6];
    _MotorSpeed_buf[9]  = _MotorSpeed_data[7];
    _MotorSpeed_buf[10] = _MotorSpeed_data[8];
    _MotorSpeed_buf[11] = XorCheck(_MotorSpeed_data, 9);


    comSendBuf(COM1, _MotorSpeed_buf, 12);
}
/*
*********************************************************************************************************
*	函 数 名: CarSpeed_Feedback
*	功能说明: 小车速度及转向角度反馈（报头:0xFD 报文长度:0x0A 功能位:0x02）
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void CarSpeedInfoUpdate()
{

    uint8_t _ucCarSpeedbuf[13] = {0};

    uint16_t _usCarSpeedX = 0;
    uint16_t _usCarSpeedY = 0;

    uint16_t _usAngularVelocity = 0;

    //		printf("g_EncoderValue.MotorA_EncoderValue.dir= %d g_EncoderValue.MotorC_EncoderValue.dir= %d \r\n", g_EncoderValue.MotorA_EncoderValue.dir,g_EncoderValue.MotorD_EncoderValue.dir);

    if((g_EncoderValue.MotorA_EncoderValue.dir == 0) && (g_EncoderValue.MotorC_EncoderValue.dir == 1)) //左右轮胎轮子都朝前运动
    {
        _ucCarSpeedbuf[3] = 0;
        _usCarSpeedX      = (g_WheelSpeed[0] + g_WheelSpeed[2]) * 0.5;

        if(g_WheelSpeed[0] >= g_WheelSpeed[2])
        {
            _ucCarSpeedbuf[9]  = 1;
            _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[0] - g_WheelSpeed[2]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));
        }
        else
        {
            _ucCarSpeedbuf[9] = 0;

            _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[2] - g_WheelSpeed[0]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));
        }
    }
    if((g_EncoderValue.MotorA_EncoderValue.dir == 1) && (g_EncoderValue.MotorC_EncoderValue.dir == 0)) //左右轮胎轮子都朝后运动
    {

        _ucCarSpeedbuf[3] = 1;
        _usCarSpeedX = (g_WheelSpeed[0] + g_WheelSpeed[2]) * 0.5;

        //			        printf("_usCarSpeedX  = %d \r\n", _usCarSpeedX);
        if(g_WheelSpeed[0] >= g_WheelSpeed[2])
        {
            _ucCarSpeedbuf[9]  = 1;

            _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[0] - g_WheelSpeed[2]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));
        }
        else
        {
            _ucCarSpeedbuf[9] = 0;

            _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[2] - g_WheelSpeed[0]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));
        }
    }
    if((g_EncoderValue.MotorA_EncoderValue.dir == 0) && (g_EncoderValue.MotorC_EncoderValue.dir == 0)) //左侧轮子朝前运动，右侧轮子朝后运动
    {
        _ucCarSpeedbuf[9] = 1;

        _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[2] + g_WheelSpeed[0]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));

        if(g_WheelSpeed[0] >= g_WheelSpeed[2])
        {
            _ucCarSpeedbuf[3] = 0;
            _usCarSpeedX = (g_WheelSpeed[0] - g_WheelSpeed[2]) * 0.5;
        }
        else
        {
            _ucCarSpeedbuf[3] = 1;
            _usCarSpeedX = (g_WheelSpeed[2] - g_WheelSpeed[0]) * 0.5;
        }

    }
    if((g_EncoderValue.MotorA_EncoderValue.dir == 1) && (g_EncoderValue.MotorC_EncoderValue.dir == 1)) //左侧轮子朝后运动，右侧轮子朝前运动
    {
        _ucCarSpeedbuf[9] = 0;
        _usAngularVelocity = (uint16_t)((100 * (g_WheelSpeed[2] + g_WheelSpeed[0]) * TIRE_SPACE) / (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE));
        if(g_WheelSpeed[0] >= g_WheelSpeed[2])
        {
            _ucCarSpeedbuf[3] = 1;
            _usCarSpeedX = (g_WheelSpeed[0] - g_WheelSpeed[2]) * 0.5;
        }
        else
        {
            _ucCarSpeedbuf[3] = 0;
            _usCarSpeedX = (g_WheelSpeed[2] - g_WheelSpeed[0]) * 0.5;
        }
    }
    _ucCarSpeedbuf[0] = 0xFD;
    _ucCarSpeedbuf[1] = 0x0A;
    _ucCarSpeedbuf[2] = 02;
    _ucCarSpeedbuf[4] = (_usCarSpeedX >> 8) & 0xFF;
    _ucCarSpeedbuf[5] = _usCarSpeedX & 0xFF;
    _ucCarSpeedbuf[6] = 00;
    _ucCarSpeedbuf[7] = (_usCarSpeedY >> 8) & 0xFF;
    _ucCarSpeedbuf[8] = _usCarSpeedY & 0xFF;


    _ucCarSpeedbuf[10] = (_usAngularVelocity >> 8) & 0xFF;
    _ucCarSpeedbuf[11] = _usAngularVelocity & 0xFF;

    _ucCarSpeedbuf[12] = xorCheckCalc(_ucCarSpeedbuf, 2, 10);


    comSendBuf(COM1, _ucCarSpeedbuf, 13);
}

/*
*********************************************************************************************************
*	函 数 名: Robot_Model_Feedback
*	功能说明: 底盘型号 ：0x01 阿克曼车型  0x02四轮差速车 0x03麦克纳母轮车 0x04履带车 0x05 两轮差速车 0x06 三轮万向车
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void RobotModelInfoUpdate()
{
    uint8_t _ucaRobotModelBuf[5] = {0};

    _ucaRobotModelBuf[0] = 0xFD;
    _ucaRobotModelBuf[1] = 0x02;
    _ucaRobotModelBuf[2] = 0x07; //功能位0x07
    _ucaRobotModelBuf[3] = 0x01;
    _ucaRobotModelBuf[4] = xorCheckCalc(_ucaRobotModelBuf, 2, 2);
    comSendBuf(COM1, _ucaRobotModelBuf, 5);
}