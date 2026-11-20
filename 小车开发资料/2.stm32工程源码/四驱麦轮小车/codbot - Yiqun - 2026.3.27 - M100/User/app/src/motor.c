/*
*********************************************************************************************************
*
*	模块名称 : 电机控制
*	文件名称 : moto.c
*	版    本 : V1.1
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/

#include "includes.h"

extern int16_t g_WheelSpeed[4]; //转速
extern int16_t Send_WheelSpeed[4];

/*
*********************************************************************************************************
*	函 数 名: MotorControl
*	功能说明: 位机运动控制
*	形    参: _ucAngularDirection转向方向 1右2左,_ucAngularSpeed转向速度, _ucMotionDirection运动方向, _usMotionSpeed运动速度
*	返 回 值: 无
*********************************************************************************************************
*/
int16_t _sMotorA_Speed_M = 0;
int16_t _sMotorB_Speed_M = 0;
int16_t _sMotorC_Speed_M = 0;
int16_t _sMotorD_Speed_M = 0;
void MotorControl(uint8_t _ucAngularDirection, uint16_t _ucAngularSpeed, uint8_t _ucMotionDirection, uint16_t _usMotionSpeed, uint8_t _ucMotionDirection_Y, uint16_t _usMotionSpeed_Y)
{
	  int16_t _usMotorA_Speed_M = 0;
    int16_t _usMotorB_Speed_M = 0;
    int16_t _usMotorC_Speed_M = 0;
    int16_t _usMotorD_Speed_M = 0;
	
		int16_t _usMotionSpeed_M = 0;
		int16_t _ucAngularSpeed_M = 0;
	
		int16_t _usMotionSpeed_Y_M = 0;

	
    uint16_t _usMotorA_Speed = 0;
    uint16_t _usMotorB_Speed = 0;
    uint16_t _usMotorC_Speed = 0;
    uint16_t _usMotorD_Speed = 0;

    uint16_t _usMotorA_PWM = 0;
    uint16_t _usMotorB_PWM = 0;
    uint16_t _usMotorC_PWM = 0;
    uint16_t _usMotorD_PWM = 0;

    uint8_t _ucMotorA_Direction = 0;
    uint8_t _ucMotorB_Direction = 0;
    uint8_t _ucMotorC_Direction = 0;
    uint8_t _ucMotorD_Direction = 0;

    uint16_t _usLinearSpeed = 0; //根据角速度计算的线速度
		
		if(_ucAngularDirection==0)
		{
		
		_ucAngularSpeed_M=_ucAngularSpeed;
		}
		else
		{
				_ucAngularSpeed_M=-_ucAngularSpeed;
		}
		if(_ucMotionDirection==0)
		{		
		 _usMotionSpeed_M=_usMotionSpeed;
		}
		else
		{		
			 _usMotionSpeed_M=-_usMotionSpeed;	
		}
				if(_ucMotionDirection_Y==0)
		{		
		 _usMotionSpeed_Y_M=-_usMotionSpeed_Y;
		}
		else
		{		
			 _usMotionSpeed_Y_M=_usMotionSpeed_Y;	
		}

    if((_ucAngularSpeed == 0) && (_usMotionSpeed == 0)&& (_usMotionSpeed_Y == 0))//当设定的角速度或者运动速度为0时 对PID的累计误差进行清0
    {
        PID_Param_SetZero();

    }

    if(_usMotionSpeed >= AUTO_SPEED_MAX)//行驶速度限制
    {
        _usMotionSpeed = AUTO_SPEED_MAX;
    }
		if(_usMotionSpeed_Y >= AUTO_SPEED_MAX)//行驶速度限制
    {
        _usMotionSpeed_Y = AUTO_SPEED_MAX;
    }
		_ucAngularSpeed_M = (int16_t)(_ucAngularSpeed_M * 0.01 * (110+92.5));//92.5=轮胎轴距*0.5，110=两轮胎宽度*0.5
		
		_sMotorA_Speed_M = _usMotionSpeed_M+_usMotionSpeed_Y_M-_ucAngularSpeed_M;//麦克纳姆轮底盘转向模型
    _sMotorB_Speed_M = _usMotionSpeed_M-_usMotionSpeed_Y_M-_ucAngularSpeed_M;
    _sMotorC_Speed_M =  _usMotionSpeed_M-_usMotionSpeed_Y_M+_ucAngularSpeed_M;
    _sMotorD_Speed_M = _usMotionSpeed_M+_usMotionSpeed_Y_M+_ucAngularSpeed_M;	
	
//printf("%d   %d   %d  %d\r\n",_usMotorA_Speed_M,_usMotorB_Speed_M,_usMotorC_Speed_M,_usMotorD_Speed_M);		
		
		
}

/***********************************************************
*@fuction	:PID_SetPWM
*@brief		:将计算好的PWM输出给电机
*@param		:int16_t _sMotorX
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-07-07
***********************************************************/

void PID_SetPWM(int16_t _sMotorA, int16_t _sMotorB, int16_t _sMotorC, int16_t _sMotorD)
{
    if(_sMotorA > 0)
    {
        MotoASetSpeed(0, _sMotorA);
    }
    if(_sMotorA <= 0)
    {
        MotoASetSpeed(1, -1 * _sMotorA);
    }

    if(_sMotorB > 0)
    {
        MotoBSetSpeed(0, _sMotorB);
    }
    if(_sMotorB <= 0)
    {
        MotoBSetSpeed(1, -1 * _sMotorB);
    }

    if(_sMotorC > 0)
    {
        MotoCSetSpeed(1, _sMotorC);
    }
    if(_sMotorC <= 0)
    {
        MotoCSetSpeed(0, -1 * _sMotorC);
    }

    if(_sMotorD > 0)
    {
        MotoDSetSpeed(1, _sMotorD);
    }
    if(_sMotorD <= 0)
    {
        MotoDSetSpeed(0, -1 * _sMotorD);
    }
}
#if 0  /*暂时没用*/
/*
*********************************************************************************************************
*	函 数 名: MotorControl_Auto
*	功能说明: 后桥电机上位机控制
*	形    参: _SteerDirection转向方向,_SteerAngle转向角度, _MotionDirection运动方向, _MotionSpeed运动速度
*	返 回 值: 无
*********************************************************************************************************
*/
void ble_motorControl(uint8_t _angular_Direction, uint8_t _angular_speed, uint8_t _MotionDirection, uint16_t _MotionSpeed)
{
    uint16_t _MotorA_Speed = 0;
    uint16_t _MotorB_Speed = 0;
    uint16_t _MotorC_Speed = 0;
    uint16_t _MotorD_Speed = 0;

    uint16_t _MotorA_PWM = 0;
    uint16_t _MotorB_PWM = 0;
    uint16_t _MotorC_PWM = 0;
    uint16_t _MotorD_PWM = 0;

    uint8_t _MotorA_Direction = 0;
    uint8_t _MotorB_Direction = 0;
    uint8_t _MotorC_Direction = 0;
    uint8_t _MotorD_Direction = 0;

    uint16_t _linear_speed = 0; //根据角速度计算的线速度

    if((_angular_speed == 0) && (_MotionSpeed == 0))
    {
        PID_Param_SetZero();

    }


    if(_MotionSpeed >= AUTO_SPEED_MAX) //行驶速度限制
    {
        _MotionSpeed = AUTO_SPEED_MAX;
    }

    if(_angular_speed == 0) //直线行驶
    {
        if(_MotionDirection == 0)
        {
            _MotorA_Direction = 0;
            _MotorB_Direction = 0;
            _MotorC_Direction = 1;
            _MotorD_Direction = 1;
        }
        else
        {
            _MotorA_Direction = 1;
            _MotorB_Direction = 1;
            _MotorC_Direction = 0;
            _MotorD_Direction = 0;
        }
        _MotorA_Speed = _MotionSpeed;
        _MotorB_Speed = _MotionSpeed;
        _MotorC_Speed = _MotionSpeed;
        _MotorD_Speed = _MotionSpeed;
    }
    else//非直线行驶
    {
        //	_linear_speed=(uint16_t)(_angular_speed*0.01*0.185*0.5*1000);//轮胎轴距为0.185mm，根据角速度反推线速度
        _linear_speed = (uint16_t)(_angular_speed * 0.01 * (TIRE_SPACE * TIRE_SPACE + WHEELBASE * WHEELBASE) / (2 * TIRE_SPACE));
        if(_angular_Direction == 0) //左转弯
        {
            if(_MotionDirection == 0) //前进运动
            {
                _MotorC_Direction = 1;
                _MotorD_Direction = _MotorC_Direction; //同一侧轮子运动方向相同
                _MotorC_Speed = _linear_speed + _MotionSpeed;
                _MotorD_Speed = _MotorC_Speed; //同一侧轮子运动速度相等

                if(_MotionSpeed >= _linear_speed)
                {
                    _MotorA_Direction = 0;
                    _MotorB_Direction = _MotorA_Direction;
                    _MotorA_Speed = _MotionSpeed - _linear_speed;
                    _MotorB_Speed = _MotorA_Speed;
                }
                else
                {
                    _MotorA_Direction = 1;
                    _MotorB_Direction = _MotorA_Direction;
                    _MotorA_Speed = _linear_speed - _MotionSpeed;
                    _MotorB_Speed = _MotorA_Speed;

                }

            }
            else//倒车运动
            {
                _MotorC_Direction = 0;
                _MotorD_Direction = _MotorC_Direction; //同一侧轮子运动方向相同
                _MotorC_Speed = _linear_speed + _MotionSpeed;
                _MotorD_Speed = _MotorC_Speed; //同一侧轮子运动速度相等

                if(_MotionSpeed >= _linear_speed)
                {
                    _MotorA_Direction = 1;
                    _MotorB_Direction = _MotorA_Direction;
                    _MotorA_Speed = _MotionSpeed - _linear_speed;
                    _MotorB_Speed = _MotorA_Speed;
                }
                else
                {
                    _MotorA_Direction = 0;
                    _MotorB_Direction = _MotorA_Direction;
                    _MotorA_Speed = _linear_speed - _MotionSpeed;
                    _MotorB_Speed = _MotorA_Speed;

                }

            }


        }
        else//右转弯
        {
            if(_MotionDirection == 0) //前进运动
            {
                _MotorA_Direction = 0;
                _MotorB_Direction = _MotorA_Direction; //同一侧轮子运动方向相同
                _MotorA_Speed = _linear_speed + _MotionSpeed;
                _MotorB_Speed = _MotorA_Speed; //同一侧轮子运动速度相等
                if(_MotionSpeed >= _linear_speed)
                {
                    _MotorC_Direction = 1;
                    _MotorD_Direction = _MotorC_Direction;
                    _MotorC_Speed = _MotionSpeed - _linear_speed;
                    _MotorD_Speed = _MotorC_Speed;
                }
                else
                {
                    _MotorC_Direction = 0;
                    _MotorD_Direction = _MotorC_Direction;
                    _MotorC_Speed = _linear_speed - _MotionSpeed;
                    _MotorD_Speed = _MotorC_Speed;

                }

            }
            else//倒车运动
            {
                _MotorA_Direction = 1;
                _MotorB_Direction = _MotorA_Direction; //同一侧轮子运动方向相同
                _MotorA_Speed = _linear_speed + _MotionSpeed;
                _MotorB_Speed = _MotorA_Speed; //同一侧轮子运动速度相等
                if(_MotionSpeed >= _linear_speed)
                {
                    _MotorC_Direction = 0;
                    _MotorD_Direction = _MotorC_Direction;
                    _MotorC_Speed = _MotionSpeed - _linear_speed;
                    _MotorD_Speed = _MotorC_Speed;
                }
                else
                {
                    _MotorC_Direction = 1;
                    _MotorD_Direction = _MotorC_Direction;
                    _MotorC_Speed = _linear_speed - _MotionSpeed;
                    _MotorD_Speed = _MotorC_Speed;

                }

            }



        }



    }



    _MotorA_PWM = wheelSpeedPidCalc(0, g_WheelSpeed[0],  _MotorA_Speed); //执行PID计算
    _MotorB_PWM = _MotorA_PWM;
    _MotorC_PWM = wheelSpeedPidCalc(2, g_WheelSpeed[2],  _MotorC_Speed); //执行PID计算
    _MotorD_PWM = _MotorC_PWM; //执行PID计算
    //		printf("%d   %d   %d  %d \r\n",_MotorA_PWM,_MotorB_PWM,_MotorC_PWM,_MotorD_PWM);
    //电机速度设置为0时，设置对应PWM=0
    if(_MotorA_Speed == 0)
    {
        _MotorA_PWM = 0;
    }
    if(_MotorB_Speed == 0)
    {
        _MotorB_PWM = 0;
    }
    if(_MotorC_Speed == 0)
    {
        _MotorC_PWM = 0;
    }
    if(_MotorD_Speed == 0)
    {
        _MotorD_PWM = 0;
    }
    //		printf("%d   %d   %d  %d   %d   %d   %d  %d   %d   %d   %d  %d\r\n",_MotorA_Speed,_MotorB_Speed,_MotorC_Speed,_MotorD_Speed,_MotorA_PWM,_MotorB_PWM,_MotorC_PWM,_MotorD_PWM,g_WheelSpeed[0],g_WheelSpeed[1],g_WheelSpeed[2],g_WheelSpeed[3]);

    //根据PID计算的PWM值驱动电机
    MotoASetSpeed(_MotorA_Direction, _MotorA_PWM);
    MotoBSetSpeed(_MotorB_Direction, _MotorB_PWM);
    MotoCSetSpeed(_MotorC_Direction, _MotorC_PWM);
    MotoDSetSpeed(_MotorC_Direction, _MotorD_PWM);

}




/*
*********************************************************************************************************
*	函 数 名: MotorControl_Ps2
*	功能说明: 后桥电机手柄控制
*	形    参: _Ps2PadXValue 手柄左摇杆X轴方向的值  _Ps2PadYValue 手柄右摇杆Y轴方向的值
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorControl_Ps2(uint8_t _Ps2PadXValue, uint8_t  _Ps2PadYValue)
{
    uint16_t _LeftMotor_Speed = 0;
    uint16_t _RightMotor_Speed = 0;
    uint8_t _MotionDirection = 0;
    uint8_t _SteerAngle = 0;
    uint16_t _LeftMotor_PWM = 0;
    uint16_t _RightMotor_PWM = 0;
    uint8_t _MotorA_Direction = 0;
    uint8_t _MotorB_Direction = 0;


    if(_Ps2PadXValue <= 138 & _Ps2PadXValue >= 118) //直线行驶
    {
        if(_Ps2PadYValue == 255) //后退
        {
            _MotionDirection = 1;
            _LeftMotor_Speed = PS2_SPEED_MAX * (_Ps2PadYValue - 127) / 127;

        }
        if(_Ps2PadYValue == 0) //前进
        {
            _MotionDirection = 0;
            _LeftMotor_Speed = PS2_SPEED_MAX * (127 - _Ps2PadYValue) / 127;
        }
        if(_Ps2PadYValue <= 200 & _Ps2PadYValue >= 50)
        {
            _LeftMotor_Speed = 0;
            _LeftMotor_Speed = 0;
        }

        _RightMotor_Speed = _LeftMotor_Speed;
    }
    if(_Ps2PadXValue < 117) //左转弯行驶
    {
        _SteerAngle = EPS_RANGE * (128 - _Ps2PadXValue) / 128;
        if(_Ps2PadYValue == 255) //后退
        {
            _MotionDirection = 1;
            _RightMotor_Speed = PS2_SPEED_MAX * (_Ps2PadYValue - 127) / 127;
            _LeftMotor_Speed = _RightMotor_Speed / (0.96 + 0.0183 * _SteerAngle);

        }
        if(_Ps2PadYValue == 0) //前进
        {
            _MotionDirection = 0;
            _RightMotor_Speed = PS2_SPEED_MAX * (127 - _Ps2PadYValue) / 127;
            _LeftMotor_Speed = _RightMotor_Speed / (0.96 + 0.0183 * _SteerAngle);
        }
        if(_Ps2PadYValue <= 200 & _Ps2PadYValue >= 50)
        {
            _LeftMotor_Speed = 0;
            _LeftMotor_Speed = 0;
        }
    }
    if(_Ps2PadXValue > 138) //右转弯行驶
    {
        _SteerAngle = EPS_RANGE * (_Ps2PadXValue - 128) / 128;
        if(_Ps2PadYValue == 255) //后退
        {
            _MotionDirection = 1;
            _LeftMotor_Speed = PS2_SPEED_MAX * (_Ps2PadYValue - 127) / 127;
            _RightMotor_Speed = _LeftMotor_Speed / (0.96 + 0.0183 * _SteerAngle); //阿克曼转向原理
        }
        if(_Ps2PadYValue == 0) //前进
        {
            _MotionDirection = 0;
            _LeftMotor_Speed = PS2_SPEED_MAX * (127 - _Ps2PadYValue) / 127;
            _RightMotor_Speed = _LeftMotor_Speed / (0.96 + 0.0183 * _SteerAngle); //阿克曼转向原理
        }
        if(_Ps2PadYValue <= 200 & _Ps2PadYValue >= 50)
        {
            _LeftMotor_Speed = 0;
            _LeftMotor_Speed = 0;
        }
    }
    if(_LeftMotor_Speed >= PS2_SPEED_MAX)
    {
        _LeftMotor_Speed = PS2_SPEED_MAX;
    }
    if(_RightMotor_Speed >= PS2_SPEED_MAX)
    {
        _RightMotor_Speed = PS2_SPEED_MAX;
    }
    _LeftMotor_PWM = wheelSpeedPidCalc(0, g_WheelSpeed[0],  _LeftMotor_Speed);


    _RightMotor_PWM = wheelSpeedPidCalc(2, g_WheelSpeed[1],  _RightMotor_Speed);

    //电机速度设置为0时，设置对应PWM=0
    if(_LeftMotor_Speed == 0)
    {
        _LeftMotor_PWM = 0;
    }
    if(_RightMotor_Speed == 0)
    {
        _RightMotor_PWM = 0;
    }
    //小车左右轮对称安装，运动方向相反
    if(_MotionDirection == 0)
    {
        _MotorA_Direction = 0;
        _MotorB_Direction = 1;
    }
    if(_MotionDirection == 1)
    {
        _MotorA_Direction = 1;
        _MotorB_Direction = 0;
    }
    //根据PID计算的PWM值驱动电机
    MotoASetSpeed(_MotorA_Direction, _LeftMotor_PWM);
    MotoBSetSpeed(_MotorB_Direction, _RightMotor_PWM);


    // printf("%d   %d    %d    %d    %d    %d\r\n",_LeftMotor_Speed,g_WheelSpeed[0],_LeftMotor_PWM,_RightMotor_Speed ,g_WheelSpeed[1],_RightMotor_PWM);
    //	printf("---%d---%d---%d---%d --- %d\r\n",g_WheelSpeed[0],g_WheelSpeed[1],g_WheelSpeed[2],g_WheelSpeed[3],_SteerAngle);
    //printf("---%d---%d---%d---%d --- %d---%d---%d---%d\r\n",g_WheelSpeed[0],g_WheelSpeed[1],g_WheelSpeed[2],g_WheelSpeed[3],g_EncoderValue.MotorA_EncoderValue.dir,g_EncoderValue.MotorB_EncoderValue.dir,g_EncoderValue.MotorC_EncoderValue.dir,g_EncoderValue.MotorD_EncoderValue.dir);
}

#endif























