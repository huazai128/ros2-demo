/*
*********************************************************************************************************
*
*	模块名称 : CCD巡线相关
*	文件名称 : bsp_ccd.c
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
#define TSL_SI    PBout(11)   //SI  
#define TSL_CLK   PAout(11)   //CLK 
/*variate----------------------------------------------------------------------*/
ADC_HandleTypeDef ADC_CCD_Handler;		//ADC句柄
uint16_t CCD_Val[128] = {0};
uint8_t CCD_MidVal = 0, CCD_Threshold = 0;

/*statement--------------------------------------------------------------------*/


/*Function prototype Begin*****************************************************/

/***********************************************************
*@fuction	:bsp_Init_CCD_Adc
*@brief		:CCD AO ADC初始化
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

void bsp_Init_CCD_Adc(void)
{
    RCC_PeriphCLKInitTypeDef ADC_CLKInit;

    ADC_CLKInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;			//ADC外设时钟
    ADC_CLKInit.AdcClockSelection = RCC_ADCPCLK2_DIV2;			//分频因子6时钟为72M/6=12MHz
    HAL_RCCEx_PeriphCLKConfig(&ADC_CLKInit);					//设置ADC时钟

    ADC_CCD_Handler.Instance = ADC1;
    ADC_CCD_Handler.Init.DataAlign = ADC_DATAALIGN_RIGHT;           //右对齐
    ADC_CCD_Handler.Init.ScanConvMode = DISABLE;                    //非扫描模式
    ADC_CCD_Handler.Init.ContinuousConvMode = DISABLE;              //关闭连续转换
    ADC_CCD_Handler.Init.NbrOfConversion = 1;                       //1个转换在规则序列中 也就是只转换规则序列1
    ADC_CCD_Handler.Init.DiscontinuousConvMode = DISABLE;           //禁止不连续采样模式
    ADC_CCD_Handler.Init.NbrOfDiscConversion = 0;                   //不连续采样通道数为0
    ADC_CCD_Handler.Init.ExternalTrigConv = ADC_SOFTWARE_START;     //软件触发
    HAL_ADC_Init(&ADC_CCD_Handler);                                 //初始化

    HAL_ADCEx_Calibration_Start(&ADC_CCD_Handler);					 //校准ADC
}

/***********************************************************
*@fuction	:bsp_InitCCD
*@brief		:CCD GPIO初始化
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

void bsp_InitCCD(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC1_CLK_ENABLE();            //使能ADC1时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOA时钟
    GPIO_Initure.Pin = TSL1401_CCD_AO_Pin;
    GPIO_Initure.Mode = GPIO_MODE_ANALOG;   //模拟
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    HAL_GPIO_Init(TSL1401_CCD_AO_Port, &GPIO_Initure);
	
		__HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
    GPIO_Initure.Pin = TSL1401_CCD_SI_Pin | TSL1401_CCD_CLK_Pin;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_NOPULL;        //不带上下拉
    GPIO_Initure.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TSL1401_CCD_SI_Port, &GPIO_Initure);
		HAL_GPIO_Init(TSL1401_CCD_CLK_Port, &GPIO_Initure);	
    bsp_Init_CCD_Adc();
}

/***********************************************************
*@fuction	:getCCDAdcvalue
*@brief		:获取ADC值
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

uint16_t getCCDAdcvalue(uint32_t ch)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

    ADC1_ChanConf.Channel = ch;                                 //通道
    ADC1_ChanConf.Rank = 1;                                     //第1个序列，序列1
    ADC1_ChanConf.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;    //采样时间
    HAL_ADC_ConfigChannel(&ADC_CCD_Handler, &ADC1_ChanConf);       //通道配置

    HAL_ADC_Start(&ADC_CCD_Handler);                               //开启ADC

    HAL_ADC_PollForConversion(&ADC_CCD_Handler, 10);               //轮询转换

    return (uint16_t)HAL_ADC_GetValue(&ADC_CCD_Handler);	        	//返回最近一次ADC1规则组的转换结果
}

/***********************************************************
*@fuction	:Read_CCD_Val
*@brief		:读取一次CCD
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

void Read_CCD_Val(void) //具体的采集时序请参考用户开发手册“TSL1401线性CCD模块开发手册”，里面有采集时序的详细说明
{
    u8 i = 0, tslp = 0;
    TSL_CLK = 0;
    TSL_SI = 0;
    bsp_DelayUS(20);

    TSL_SI = 1;
    TSL_CLK = 0;
    bsp_DelayUS(20);

    TSL_CLK = 1;
    bsp_DelayUS(20);
    TSL_SI = 0;

    for(i = 0; i < 128; i++)
    {
        TSL_CLK = 0;
        bsp_DelayUS(20);
        CCD_Val[tslp] = (getCCDAdcvalue(15)) >> 4;
        ++tslp;
        TSL_CLK = 1;
        bsp_DelayUS(20);
    }
}

/***********************************************************
*@fuction	:myabs
*@brief		:取绝对值
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

int myabs(int a)
{
    int temp;
    if(a < 0)  temp = -a;
    else temp = a;
    return temp;
}

/***********************************************************
*@fuction	:Find_CCD_MidVal
*@brief		:
*@param		:--
*@return	:void
*@author	:Hades_Czq
*@date		:2023-08-14
***********************************************************/

void  Find_CCD_MidVal(void)
{
    static u8 i, j, Left, Right, Last_CCD_MidVal;
    static u16 value1_max, value1_min;

    value1_max = CCD_Val[0]; //动态阈值算法，读取最大和最小值
    for(i = 5; i < 123; i++) //两边各去掉5个点
    {
        if(value1_max <= CCD_Val[i])
            value1_max = CCD_Val[i];
    }
    value1_min = CCD_Val[0]; //最小值
    for(i = 5; i < 123; i++)
    {
        if(value1_min >= CCD_Val[i])
            value1_min = CCD_Val[i];
    }
    CCD_Threshold = (value1_max + value1_min) / 2;	 //计算出本次中线提取的阈值
    for(i = 5; i < 118; i++) //寻找左边跳变沿
    {
        if(CCD_Val[i] > CCD_Threshold && CCD_Val[i + 1] > CCD_Threshold && CCD_Val[i + 2] > CCD_Threshold && CCD_Val[i + 3] < CCD_Threshold && CCD_Val[i + 4] < CCD_Threshold && CCD_Val[i + 5] < CCD_Threshold)
        {
            Left = i;
            break;
        }
    }
    for(j = 118; j > 5; j--) //寻找右边跳变沿
    {
        if(CCD_Val[j] < CCD_Threshold && CCD_Val[j + 1] < CCD_Threshold && CCD_Val[j + 2] < CCD_Threshold && CCD_Val[j + 3] > CCD_Threshold && CCD_Val[j + 4] > CCD_Threshold && CCD_Val[j + 5] > CCD_Threshold)
        {
            Right = j;
            break;
        }
    }
    CCD_MidVal = (Right + Left) / 2; //计算中线位置
    if(myabs(CCD_MidVal - Last_CCD_MidVal) > 70) //计算中线的偏差，如果太大
        CCD_MidVal = Last_CCD_MidVal;  //则取上一次的值
    Last_CCD_MidVal = CCD_MidVal; //保存上一次的偏差
}

/*Function prototype End*******************************************************/





