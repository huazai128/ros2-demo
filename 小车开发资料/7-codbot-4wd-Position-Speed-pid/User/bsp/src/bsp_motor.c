/*
*********************************************************************************************************
*
*	模块名称 : 电机驱动
*	文件名称 : bsp_motor.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*
*********************************************************************************************************
*/
#include "bsp.h"

/*
*********************************************************************************************************
*	函 数 名: bsp_InitMotorEnGpio
*	功能说明: TB6612的IN引脚初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitMotorEnGpio(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 第1步：打开GPIO时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio_init.Mode  = GPIO_MODE_OUTPUT_PP;	  /* 设置开漏输出 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO速度等级 */
    gpio_init.Pin = GPIO_PIN_13;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_14;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &gpio_init);



    gpio_init.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_10;
    HAL_GPIO_Init(GPIOC, &gpio_init);
}
/*
*********************************************************************************************************
*	函 数 名: MotorA_Forward
*	功能说明: 电机A正转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorA_Forward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);


}
/*
*********************************************************************************************************
*	函 数 名: MotorA_Forward
*	功能说明: 电机A反转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorA_Backward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);

}
/*
*********************************************************************************************************
*	函 数 名: MotorB_Forward
*	功能说明: 电机B正转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorB_Forward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);


}
/*
*********************************************************************************************************
*	函 数 名: MotorB_Backward
*	功能说明: 电机B反转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorB_Backward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);



}
/*
*********************************************************************************************************
*	函 数 名: MotorC_Forward
*	功能说明: 电机C正转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorC_Forward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);

}
/*
*********************************************************************************************************
*	函 数 名: MotorC_Backward
*	功能说明: 电机C反转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorC_Backward(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);


}
/*
*********************************************************************************************************
*	函 数 名: MotorD_Forward
*	功能说明: 电机D正转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorD_Forward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);

}
/*
*********************************************************************************************************
*	函 数 名: MotorD_Backward
*	功能说明: 电机D反转
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MotorD_Backward(void)
{
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);

}

/*
*********************************************************************************************************
*	函 数 名: MotoASetSpeed
*	功能说明: A电机的占空比设置
*	形    参: _direction ：运动方向 00表示正转 01表示反转 _speed：占空比值 范围0~2000
*	返 回 值: 无
*********************************************************************************************************
*/

void MotoASetSpeed(int8_t _direction, int16_t _speed)
{
    if(_direction == 0)

    {

        MotorA_Forward();

        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_1, abs(_speed));


    }
    else
    {
        MotorA_Backward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_1, abs(_speed));
    }

}

/*
*********************************************************************************************************
*	函 数 名: MotoBSetSpeed
*	功能说明: B电机的占空比设置
*	形    参: _direction ：运动方向 00表示正转 01表示反转 _speed：占空比值 范围0~2000
*	返 回 值: 无
*********************************************************************************************************
*/

void MotoBSetSpeed(int8_t _direction, int16_t _speed)
{
    if(_direction == 0)
    {
        MotorB_Forward();

        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_2, abs(_speed));

    }
    else
    {
        MotorB_Backward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_2, abs(_speed));
    }

}
/*
*********************************************************************************************************
*	函 数 名: MotoCSetSpeed
*	功能说明: C电机的占空比设置
*	形    参: _direction ：运动方向 00表示正转 01表示反转 _speed：占空比值 范围0~2000
*	返 回 值: 无
*********************************************************************************************************
*/

void MotoCSetSpeed(int8_t _direction, int16_t _speed)
{
    if(_direction == 0)
    {

        MotorC_Forward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_3, abs(_speed));

    }
    else
    {
        MotorC_Backward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_3, abs(_speed));
    }

}
/*
*********************************************************************************************************
*	函 数 名: MotoDSetSpeed
*	功能说明: D电机的占空比设置
*	形    参: _direction ：运动方向 00表示正转 01表示反转 _speed：占空比值 范围0~2000
*	返 回 值: 无
*********************************************************************************************************
*/

void MotoDSetSpeed(int8_t _direction, int16_t _speed)
{
    if(_direction == 0)
    {
        MotorD_Forward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_4, abs(_speed));

    }
    else
    {
        MotorD_Backward();
        __HAL_TIM_SET_COMPARE(&htim3Motor, TIM_CHANNEL_4, abs(_speed));
    }

}

