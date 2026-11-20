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
*@brief		:四路巡线
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
//		SEGGER_RTT_printf(0 ,"level--%d %d %d %d %d \r\n",_level.l2 ,_level.l1 , _level.m ,_level.r1);
        if(_level.l2 && _level.l1  && !_level.m && _level.r1 )//左偏一级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  750), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  750),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400 ), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));	
						flag_zuo = 1;
					  flag_you = 0;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_zuo == 1)//左偏二级
        {
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  950), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  950),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  250), wheelSpeedPidCalc(3, g_WheelSpeed[3],  250));
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(_level.l2 && _level.l1  && _level.m && !_level.r1 && flag_zuo)//左偏三级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  1300), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  1300),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  100), wheelSpeedPidCalc(3, g_WheelSpeed[3],  100));
						flag_zuo = 2;
					  flag_you = 0;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_zuo == 2)//左偏四级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  1600), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  1600),
            wheelSpeedPidCalc(2, g_WheelSpeed[2],  0), wheelSpeedPidCalc(3, g_WheelSpeed[3],  0));
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(_level.l2 && !_level.l1  && _level.m && _level.r1)//右偏一级
        {
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  750), wheelSpeedPidCalc(3, g_WheelSpeed[3],  750));
						flag_zuo = 0;
					  flag_you = 1;
        }
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you == 1)//右偏二级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  250), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  250),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  900), wheelSpeedPidCalc(3, g_WheelSpeed[3],  900));
					  flag_zuo = 0;
					  flag_you = 1;
        }
				
        else if(!_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you)//右偏三级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  100), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  100),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  1300), wheelSpeedPidCalc(3, g_WheelSpeed[3],  1300));
            flag_zuo = 0;
					  flag_you = 2;					
        }		
				
				else if(_level.l2 && _level.l1  && _level.m && _level.r1 && flag_you == 2)//右偏四级
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  0), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  0),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  1600), wheelSpeedPidCalc(3, g_WheelSpeed[3],  1600));
            flag_zuo = 0;
					  flag_you = 1;					
        }		
				
				else//正常
        {
						PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  500), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  500),
            wheelSpeedPidCalc(2, g_WheelSpeed[2],  500), wheelSpeedPidCalc(3, g_WheelSpeed[3],  500));

            _usStopTime = 0;
        }
				
			
}


/***********************************************************
*@fuction	:gray_line_walk_task
*@brief		:5路灰度巡线
*@param		:--
*@return	:void
*@author	:CODBOT_hyj
*@date		:2024-09-1
***********************************************************/
void gray_line_walk_task(void)
{

        _level = bsp_get_line_walking();
//        if(!_level.l1 && !_level.l2  && !_level.r2 && !_level.r1 && !_level.m && !flag_zuo && !flag_you)//出界停车(meiyong)
//        {
////             PID_SetPWM(0, 0, 0, 0);
////            _usStopTime = 0;
////					  flag_zuo = 0;
////					  flag_you = 0;
//        }

        if(!_level.l1 && !_level.l2  && _level.m && _level.r2 && !_level.r1)//左偏一级
        {
//					 PID_SetPWM(900, 900, 1, 1);
				    	PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  900), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  900),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400 ), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));					   
            _usStopTime = 0;
        }
				
				else if(!_level.l1 && !_level.l2  && !_level.m && _level.r2 && !_level.r1)//左偏二级
        {
//             PID_SetPWM(1200, 1200, 1, 1);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  1000), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  1000),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l1 && !_level.l2  && !_level.m && !_level.r2 && !_level.r1 && flag_zuo)//左偏三级
        {
//             PID_SetPWM(1200, 1200, 1, 1);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  1300), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  1300),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l1 && !_level.l2  && !_level.m && !_level.r2 && _level.r1 && flag_zuo)//左偏四级
        {
//             PID_SetPWM(1500, 1500, 1, 1);
					PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  1300), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  1300),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  0), wheelSpeedPidCalc(3, g_WheelSpeed[3],  0));
            _usStopTime = 0;
						flag_zuo = 1;
					  flag_you = 0;
        }
				
        else if(!_level.l1 && _level.l2  && _level.m && !_level.r2 && !_level.r1)//右偏一级
        {
//             PID_SetPWM(1, 1, 900, 900);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  900), wheelSpeedPidCalc(3, g_WheelSpeed[3],  900));
            _usStopTime = 0;
        }
				
				else if(!_level.l1 && _level.l2  && !_level.m && !_level.r2 && !_level.r1 )//右偏二级
        {
//             PID_SetPWM(1, 1, 900, 900);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  1000), wheelSpeedPidCalc(3, g_WheelSpeed[3],  1000));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;
        }
				
        else if(!_level.l1 && !_level.l2  && !_level.m && !_level.r2 && !_level.r1 && flag_you)//右偏三级
        {
//             PID_SetPWM(1, 1, 1200, 1200);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  1300), wheelSpeedPidCalc(3, g_WheelSpeed[3],  1300));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;					
        }		
				
				else if(_level.l1 && !_level.l2  && !_level.m && !_level.r2 && !_level.r1 && flag_you)//右偏四级
        {
//             PID_SetPWM(1, 1, 1500, 1500);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  0), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  0),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  1300), wheelSpeedPidCalc(3, g_WheelSpeed[3],  1300));
            _usStopTime = 0;
						flag_zuo = 0;
					  flag_you = 1;
        }
				
				else//正常
        {
                   // PID_SetPWM(350, 350, 350, 350);
										PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0],  400), wheelSpeedPidCalc(1, -g_WheelSpeed[1],  400),
                   wheelSpeedPidCalc(2, g_WheelSpeed[2],  400), wheelSpeedPidCalc(3, g_WheelSpeed[3],  400));
            _usStopTime = 0;
//						flag_zuo = 0;
//					  flag_you = 0;
        }
				
			
}
/*Function prototype End*******************************************************/





