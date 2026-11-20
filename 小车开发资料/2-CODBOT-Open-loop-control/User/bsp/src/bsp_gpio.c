#include "bsp.h"
#include "relayControl.h"



/*********************************************************************************************************
*	函 数 名: bsp_InitGpio
*	功能说明: GPIO初始化。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitGpio(void)
{
    GPIO_InitTypeDef gpio_init;
    /* 第1步：打开GPIO时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
        
    gpio_init.Mode  = GPIO_MODE_IT_RISING_FALLING;
    //gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Pull  = GPIO_NOPULL;
        
    
    gpio_init.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &gpio_init);	
        
    gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    HAL_GPIO_Init(GPIOA, &gpio_init);
    
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
        
    HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/*********************************************************************************************************
*	函 数 名: ReadGpioLevel
*	功能说明: 读取GPIO的状态。
*	形    参: _num 引脚号
*	返 回 值: 引脚状态
*********************************************************************************************************
*/
uint8_t ReadGpioLevel(uint8_t _num)
{
    uint8_t _gpioLevel = 0;
    switch(_num)
	{
        case 1: 
            _gpioLevel =  HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14);
        break;
        case 2: 
            _gpioLevel =  HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15);
        break;
        case 3: 
            _gpioLevel =  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
        break;	
            case 4: 
            _gpioLevel =  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
        break;	
        default :
            _gpioLevel = 2;
	}
		
	return _gpioLevel;
}


/*********************************************************************************************************
*	函 数 名: EXTI15_10_IRQHandler
*	功能说明: 外部中断10 - 15处理函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

/*********************************************************************************************************
*	函 数 名: EXTI1_IRQHandler
*	功能说明: 外部中断1处理函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void EXTI1_IRQHandler(void)
{
	
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

/*********************************************************************************************************
*	函 数 名: HAL_GPIO_EXTI_Callback
*	功能说明: 外部中断回调函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	status_t _status;
	if(GPIO_Pin == GPIO_PIN_14)
	{
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_RESET)
		SetStatus(0, 1);
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_14) == GPIO_PIN_SET)
		SetStatus(0, 0);
		
	}
	else if(GPIO_Pin == GPIO_PIN_15)
	{
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_RESET)
		SetStatus(1, 1);
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == GPIO_PIN_SET)
		SetStatus(1, 0);
		
	}
	else if(GPIO_Pin == GPIO_PIN_1)
	{
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET)
		SetStatus(3, 1);
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
		SetStatus(3, 0);
	}

}

