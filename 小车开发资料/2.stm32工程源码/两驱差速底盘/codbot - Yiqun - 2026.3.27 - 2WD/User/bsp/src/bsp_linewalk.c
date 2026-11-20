/*
*********************************************************************************************************
*
*	模块名称 : 循迹模块相关（数字量）
*	文件名称 : bsp_linewalk.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*********************************************************************************************************
*/

/*include----------------------------------------------------------------------*/
#include "bsp.h"
#include "includes.h"
/*define-----------------------------------------------------------------------*/

/*variate----------------------------------------------------------------------*/

//line_walk_Level_t  _level =
//{
//    .l1 = 1,
//    .l2 = 1,
//    .r1 = 1,
//    .r2 = 1
//};
line_walk_Level_t  _level =
{
    .l1 = 1,
    .l2 = 1,
    .r1 = 1,
    .r2 = 1,
	  .m = 1
};
int16_t Speed = 500;
int16_t out_road = 0;
int16_t piancha = 0, last_piancha = 0, Stop_Flag = 0;
int16_t A_pwm = 0, B_pwm = 0, C_pwm = 0, D_pwm = 0;
int16_t turnpidout = 0;
/*statement--------------------------------------------------------------------*/


/*Function prototype Begin*****************************************************/

/***********************************************************
*@fuction	:bsp_line_work_init
*@brief		:循迹模块GPIO初始化
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-11
***********************************************************/

void bsp_line_work_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 第1步：打开GPIO时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* 定时器通道1功能引脚IO初始化 */
    GPIO_InitStruct.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin   = GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


}

/***********************************************************
*@fuction	:bsp_get_line_walking
*@brief		:获取传感器的值
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-11
***********************************************************/

//line_walk_Level_t bsp_get_line_walking(void)
//{
//    _level.l1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);//7 r1
//    _level.l2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);//6 r2
//    _level.r2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);//8 l1
//    _level.r1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3);//9 l2
//    return _level;
//}
line_walk_Level_t bsp_get_line_walking(void)
{
    _level.l2 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
    _level.l1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1);
    _level.m = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2);
    _level.r1 = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3);
	  _level.r2 = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);
    return _level;
}


/***********************************************************
*@fuction	:line_walk_task
*@brief		:巡线
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-11
***********************************************************/


int flag_zuo = 0;
int flag_you = 0;
void line_walk_task(void)
{

        _level = bsp_get_line_walking();
		SEGGER_RTT_printf(0 ,"level--%d %d %d %d %d \r\n",_level.l2 ,_level.l1 , _level.m ,_level.r1);
        if(_level.l2 && _level.l1  && !_level.m && _level.r1 )//左偏一级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  600), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  600),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400 ), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));	
						flag_zuo = 1;
					  flag_you = 0;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_zuo == 1)//左偏二级
        {
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  700), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  700),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(_level.l2 && _level.l1  && _level.m && !_level.r1 && flag_zuo)//左偏三级
        {
				PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  800), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  800),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
						flag_zuo = 2;
					  flag_you = 0;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_zuo == 2)//左偏四级
        {
PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  800), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  800),
            wheelSpeedPidCalc(2, g_WheelSpeed[2],  0), wheelSpeedPidCalc(3, g_WheelSpeed[3],  0));
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(_level.l2 && !_level.l1  && _level.m && _level.r1)//右偏一级
        {
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  600), wheelSpeedPidCalc(3, g_WheelSpeed[3],  600));
						flag_zuo = 0;
					  flag_you = 1;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you == 1)//右偏二级
        {
PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  700), wheelSpeedPidCalc(3, g_WheelSpeed[3],  700));
					  flag_zuo = 0;
					  flag_you = 1;
        }
				
        else if(!_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you)//右偏三级
        {
PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  800), wheelSpeedPidCalc(3, g_WheelSpeed[3],  800));
            flag_zuo = 0;
					  flag_you = 2;					
        }		
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you == 2)//右偏四级
        {
							PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  0), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  0),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  800), wheelSpeedPidCalc(3, g_WheelSpeed[3],  800));
            flag_zuo = 0;
					  flag_you = 1;					
        }		
				
				else//正常
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
            wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));

            _usStopTime = 0;
        }
				
			
}

void gray_line_walk_task(void)
{

        _level = bsp_get_line_walking();
	
//		SEGGER_RTT_printf(0 ,"level--%d %d %d %d %d \r\n",_level.l2 ,_level.l1 , _level.m , _level.r1 ,_level.r2);
	
//        if(!_level.l1 && !_level.l2  && !_level.r2 && !_level.r1 && !_level.m && !flag_zuo && !flag_you)//出界停车(meiyong)
//        {
////             PID_SetPWM(0, 0, 0, 0);
////            _usStopTime = 0;
////					  flag_zuo = 0;
////					  flag_you = 0;
//        }

        if(!_level.l2 && !_level.l1  && _level.m && _level.r1 && !_level.r2)//左偏一级
        {
//					SEGGER_RTT_printf(0 ,"zuo1----\r\n");
					
//					 PID_SetPWM(900, 900, 1, 1);
				    	PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  500), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  500),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400 ), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));					   
            _usStopTime = 0;
        }
				
				else if(!_level.l2 && !_level.l1  && !_level.m && _level.r1 && !_level.r2)//左偏二级
        {
//					SEGGER_RTT_printf(0 ,"zuo2----\r\n");
//             PID_SetPWM(1200, 1200, 1, 1);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  600), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  600),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l2 && !_level.l1  && !_level.m && !_level.r1 && !_level.r2 && flag_zuo)//左偏三级
        {
//					SEGGER_RTT_printf(0 ,"zuo3----\r\n");
//             PID_SetPWM(1200, 1200, 1, 1);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  800), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  800),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l2 && !_level.l1  && !_level.m && !_level.r1 && _level.r2 && flag_zuo)//左偏四级
        {
//					SEGGER_RTT_printf(0 ,"zuo4----\r\n");
//             PID_SetPWM(1500, 1500, 1, 1);
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  800), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  800),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  0), wheelSpeedPidCalc(3, g_WheelSpeed[3],  0));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l2 && _level.l1  && _level.m && !_level.r1 && !_level.r2)//右偏一级
        {
//					SEGGER_RTT_printf(0 ,"you1----\r\n");
//             PID_SetPWM(1, 1, 900, 900);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  500), wheelSpeedPidCalc(3, g_WheelSpeed[3],  500));
            _usStopTime = 0;
        }
				
				else if(!_level.l2 && _level.l1  && !_level.m && !_level.r1 && !_level.r2 )//右偏二级
        {
//					SEGGER_RTT_printf(0 ,"you2----\r\n");
//             PID_SetPWM(1, 1, 900, 900);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  600), wheelSpeedPidCalc(3, g_WheelSpeed[3],  600));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;
        }
				
        else if(!_level.l2 && !_level.l1  && !_level.m && !_level.r1 && !_level.r2 && flag_you)//右偏三级
        {
//					SEGGER_RTT_printf(0 ,"you3----\r\n");
//             PID_SetPWM(1, 1, 1200, 1200);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  800), wheelSpeedPidCalc(3, g_WheelSpeed[3],  800));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;					
        }		
				
				else if(_level.l2 && !_level.l1  && !_level.m && !_level.r1 && !_level.r2 && flag_you)//右偏四级
        {
	
//					SEGGER_RTT_printf(0 ,"you4----\r\n");
//             PID_SetPWM(1, 1, 1500, 1500);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  0), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  0),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  800), wheelSpeedPidCalc(3, g_WheelSpeed[3],  800));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;
        }
				
				else//正常
        {
//					SEGGER_RTT_printf(0 ,"zhixing----\r\n");
					
                   // PID_SetPWM(350, 350, 350, 350);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
//						flag_zuo = 0;
//					  flag_you = 0;
        }
				
			
}
/*Function prototype End*******************************************************/





