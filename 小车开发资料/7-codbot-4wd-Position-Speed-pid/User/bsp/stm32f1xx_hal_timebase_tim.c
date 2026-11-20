/*
*********************************************************************************************************
*
*	模块名称 : HAL库时基
*	文件名称 : stm32F1xx_hal_timbase_tim.c
*	版    本 : V1.0
*	说    明 : 用于为HAL库提供时间基准
*	修改记录 :
*
*********************************************************************************************************
*/

#include "includes.h"


/*
*********************************************************************************************************
*	函 数 名: HAL_Delay
*	功能说明: 重定向毫秒延迟函数。替换HAL中的函数。因为HAL中的缺省函数依赖于Systick中断，如果在USB、SD卡
*             中断中有延迟函数，则会锁死。也可以通过函数HAL_NVIC_SetPriority提升Systick中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_Delay(uint32_t Delay)
{
	bsp_DelayMS(Delay);
}

HAL_StatusTypeDef HAL_InitTick (uint32_t TickPriority)
{
	return HAL_OK;
}

uint32_t HAL_GetTick (void) 
{
	static uint32_t ticks = 0U;
	uint32_t i;

	if (osKernelGetState () == osKernelRunning)
	{
		return ((uint32_t)osKernelGetTickCount ());
	}

	/* 如果RTX5还没有运行，采用下面方式 */
	for (i = (SystemCoreClock >> 14U); i > 0U; i--) 
	{
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
		__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
	}
	
	return ++ticks;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
