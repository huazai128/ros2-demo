/*
*********************************************************************************************************
*
*	模块名称 : 超声波模块
*	文件名称 : bsp_hcsr04.c
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
/*variate----------------------------------------------------------------------*/
TIM_HandleTypeDef    htimSR04;
uint8_t  TIM4CH2_CAPTURE_STA=0;							//输入捕获状态		    				
uint16_t TIM4CH2_CAPTURE_VAL;							//输入捕获值(TIM1是16位)
extern uint8 Hinder_f;
/*statement--------------------------------------------------------------------*/


/*Function prototype Begin*****************************************************/

/***********************************************************
*@fuction	:HC_SR04_Init
*@brief		:超声波模块初始化
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-10
***********************************************************/

void HC_SR04_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct;
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/* 定时器通道1功能引脚IO初始化 */
		GPIO_InitStruct.Pin   = SR04_PIN;
		GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(SR04_PORT, &GPIO_InitStruct);
	
		GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;   			/* 设置输入 */
    GPIO_InitStruct.Pull  = GPIO_NOPULL;                   /* 上下拉电阻不使能 */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;         /* GPIO速度等级 */
    GPIO_InitStruct.Pin   = TRIG_Pin ;
    HAL_GPIO_Init(TRIG_GPIO_Port, &GPIO_InitStruct);
	
		bsp_InitTim4();
}

/*********************************************************************************************************
*	函 数 名: bsp_InitTim1
*	功能说明: Tim1功能初始化 输入捕获
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTim4(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
		TIM_IC_InitTypeDef sConfigIC;
	
    __HAL_RCC_TIM4_CLK_ENABLE();

    htimSR04.Instance           = TIM4;
    htimSR04.Init.Prescaler     = 72 - 1; //72分频
    htimSR04.Init.CounterMode   = TIM_COUNTERMODE_UP;
    htimSR04.Init.Period        = 60000-1; //60ms
    htimSR04.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htimSR04.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&htimSR04);


    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htimSR04, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;                                                                                                            
		HAL_TIMEx_MasterConfigSynchronization(&htimSR04, &sMasterConfig);
		
		sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
		sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
		sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
		sConfigIC.ICFilter = 1;
		HAL_TIM_IC_ConfigChannel(&htimSR04, &sConfigIC, TIM_CHANNEL_2);


		HAL_NVIC_SetPriority(TIM4_IRQn , 3, 1);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
    HAL_TIM_Base_Start_IT(&htimSR04);


    HAL_TIM_IC_Start_IT(&htimSR04, TIM_CHANNEL_2);
   

}

/***********************************************************
*@fuction	:HCSR_04_Start
*@brief		:给超声波模块启动信号
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-10
***********************************************************/

void HCSR_04_Start(void)
{
    uint32_t i;
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET);
    bsp_DelayUS(10);
    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);
}

/***********************************************************
*@fuction	:getSR04_fDistance
*@brief		:获取距离
*@param		:--
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-10
***********************************************************/

float getSR04_fDistance(void)
{
    float len = 0;
    float time = 0;
    if (TIM4CH2_CAPTURE_STA & 0X80) //输入捕获 触发
    {
        time = (float)(TIM4CH2_CAPTURE_STA & 0X3f);   //获得溢出次数
        time *= 60;                       //一次溢出为65536 得到溢出的时间
        time += (float)TIM4CH2_CAPTURE_VAL/1000 ;         //溢出的时间+现在定时器的值 得到总的时间
        len = (float)time * 342.62 * 100/1000 /2; // 计算得到距离

        TIM4CH2_CAPTURE_STA = 0; //清除溢出
			  TIM4CH2_CAPTURE_VAL = 0;
    }
    return len;
}

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htimSR04);
}


/***********************************************************
*@fuction	:Ultrasonic_Follow
*@brief		:超声波跟随函数
*@param		:now_dis 当前距离 target_dis 目标距离
*@return	:void
*@author	:CODBOT_Czq
*@date		:2023-08-11
***********************************************************/

void Ultrasonic_Follow(float now_dis, float target_dis)
{
//	int pidout_Follow = 0;
//	pidout_Follow = wheelFollowPidCalc(now_dis,target_dis);
//	PID_SetPWM(wheelSpeedPidCalc(0, -g_WheelSpeed[0], -pidout_Follow), wheelSpeedPidCalc(1, -g_WheelSpeed[1], -pidout_Follow), wheelSpeedPidCalc(2, g_WheelSpeed[2],  -pidout_Follow), wheelSpeedPidCalc(3, g_WheelSpeed[3],  -pidout_Follow));
}


/*Function prototype End*******************************************************/







