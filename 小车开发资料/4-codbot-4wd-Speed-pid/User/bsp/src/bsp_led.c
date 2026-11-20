/*
*********************************************************************************************************
*
*	模块名称 : LED指示灯驱动模块
*	文件名称 : bsp_led.c
*	版    本 : V1.0
*	说    明 : 驱动LED指示灯
*
*
*********************************************************************************************************
*/

#include "bsp.h"

#define ALL_LED_GPIO_CLK_ENABLE()	do{\
                                        __HAL_RCC_GPIOA_CLK_ENABLE();\
                                        __HAL_RCC_GPIOB_CLK_ENABLE();\
                                        __HAL_RCC_GPIOC_CLK_ENABLE();\
                                        __HAL_RCC_GPIOD_CLK_ENABLE();\
                                        __HAL_RCC_GPIOF_CLK_ENABLE();\
                                        }while(0);

#define GPIO_LED1	GPIOC
#define PIN_LED1	GPIO_PIN_0

#define GPIO_LED2	GPIOC
#define PIN_LED2	GPIO_PIN_1



#define LED1_ON()  	   GPIO_LED1->BSRR = ((uint32_t)PIN_LED1 << 16U)				/* LED1 = 0 */
#define LED1_OFF()     GPIO_LED1->BSRR = PIN_LED1				/* LED1 = 1 */

#define LED1_IS_ON()   ((GPIO_LED1->IDR & PIN_LED1) == 0)	/* 如果已点亮，返回1 */

#define LED2_ON()  	 GPIO_LED2->BSRR = ((uint32_t)PIN_LED2 << 16U)				/* LED1 = 0 */
#define LED2_OFF()     GPIO_LED2->BSRR = PIN_LED2				/* LED1 = 1 */

#define LED2_IS_ON()   ((GPIO_LED2->IDR & PIN_LED2) == 0)	/* 如果已点亮，返回1 */


/*
*********************************************************************************************************
*	函 数 名: bsp_InitLed
*	功能说明: 配置LED指示灯相关的GPIO,  该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitLed(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 第1步：打开GPIO时钟 */
    ALL_LED_GPIO_CLK_ENABLE();



    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* 设置开漏输出 */
    gpio_init.Pull = GPIO_NOPULL;			/* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO速度等级 */
    gpio_init.Pin = PIN_LED1;
    HAL_GPIO_Init(GPIO_LED1, &gpio_init);

    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* 设置开漏输出 */
    gpio_init.Pull = GPIO_NOPULL;			/* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO速度等级 */
    gpio_init.Pin = PIN_LED2;
    HAL_GPIO_Init(GPIO_LED2, &gpio_init);

    bsp_LedOff(1);
    bsp_LedOff(2);
}


/*
*********************************************************************************************************
*	函 数 名: bsp_LedOn
*	功能说明: 点亮指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOn(uint8_t _no)
{
    switch(_no)
    {
    case 1:
        LED1_ON();
        break;
    case 2 :
        LED2_ON();
        break;
    case 3:
        //LED3_ON();
        break;
    case 4 :
        //LED4_ON();
        break;
    }
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedOff
*	功能说明: 熄灭指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedOff(uint8_t _no)
{
    switch(_no)
    {
    case 1:
        LED1_OFF();
        break;
    case 2 :
        LED2_OFF();
        break;
    case 3:
        //LED3_OFF();
        break;
    case 4 :
        //LED4_OFF();
        break;
    }
}

/*
*********************************************************************************************************
*	函 数 名: bsp_LedToggle
*	功能说明: 翻转指定的LED指示灯。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_LedToggle(uint8_t _no)
{
    if (bsp_IsLedOn(_no))
    {
        bsp_LedOff(_no);
    }
    else
    {
        bsp_LedOn(_no);
    }
}


/*
*********************************************************************************************************
*	函 数 名: bsp_IsLedOn
*	功能说明: 判断LED指示灯是否已经点亮。
*	形    参:  _no : 指示灯序号，范围 1 - 4
*	返 回 值: 1表示已经点亮，0表示未点亮
*********************************************************************************************************
*/
uint8_t bsp_IsLedOn(uint8_t _no)
{
    switch(_no)
    {
    case 1:
        return LED1_IS_ON();
        break;
    case 2 :
        return LED2_IS_ON();
        break;
    case 3:
        //return LED3_IS_ON();
        break;
    case 4 :
        //return LED4_IS_ON();
        break;
    }
    return 0;
}





