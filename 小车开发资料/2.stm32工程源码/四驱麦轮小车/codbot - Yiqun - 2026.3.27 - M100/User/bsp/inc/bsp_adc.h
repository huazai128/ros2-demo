

#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stdint.h"

/* 供外部调用的函数声明 */
void bsp_InitAdc(void); 				//ADC通道初始化
uint16_t getAdcvalue(uint32_t ch);    		        //获得某个通道值 
uint16_t getAdcAverageValue(uint32_t ch,uint8_t times);//得到某个通道给定次数采样的平均值

#endif


