/*
*********************************************************************************************************
*
*	模块名称 : 电机编码器驱动
*	文件名称 : bsp_encoder.c
*	版    本 : V1.0
*	说    明 :
*
*   Copyright (C), 2019-2030, 武汉酷点机器人科技有限公司
*   淘宝店铺地址：https://codbot.taobao.com/
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "includes.h"

#define   REDUCTION_RATIO    30//电机减速比

Encoder_t g_EncoderValue;
int16_t g_WheelSpeed[4] = {0, 0, 0, 0} ; //数组中的每一个元素分别代表一个车轮,用于存放车轮转速
int16_t Send_WheelSpeed[4] = {0, 0, 0, 0};
float g_WheelPositon[4] = {0, 0, 0, 0};
float Send_WheelPositon[4] = {0, 0, 0, 0};
static int16_t s_EncodeValue[8] = {0};
static int32 s_position_enc_Num[8] = {0};
/*
*********************************************************************************************************
*	函 数 名: bsp_InitEncoder
*	功能说明: 编码器普通IO引脚配置
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitEncoder(void)
{
    GPIO_InitTypeDef gpio_init;

    /* 第1步：打开GPIO时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    gpio_init.Mode  = GPIO_MODE_INPUT;   			/* 设置输入 */
    gpio_init.Pull  = GPIO_NOPULL;                   /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;         /* GPIO速度等级 */
    gpio_init.Pin   = GPIO_PIN_12 | GPIO_PIN_5 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    gpio_init.Mode  = GPIO_MODE_INPUT;   			/* 设置输入 */
    gpio_init.Pull  = GPIO_NOPULL;                   /* 上下拉电阻不使能 */
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;         /* GPIO速度等级 */
    gpio_init.Pin   = GPIO_PIN_4 ;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    bsp_InitTim5();
}
/*
*********************************************************************************************************
*	函 数 名: GetSpeedInfo
*	功能说明: 把编码器测得的值转化为小车实际速度
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void GetSpeedInfo(int16_t *_speed, uint16_t _EncoderDirection1, uint16_t _EncoderDirection2, uint16_t _FeedbackDir)
{
    uint16_t _encoder = 0;

    _encoder = _EncoderDirection1 > _EncoderDirection2 ? _EncoderDirection1 : _EncoderDirection2;

    if(_EncoderDirection1 > _EncoderDirection2)
    {
        *_speed = _encoder * 20 * 3.1416 * 65  / 330; //65-轮胎直径 330-编码器线数 20-编码器获取方波的频率
    }
    else
        *_speed = -1 * _encoder * 20 * 3.1416 * 65  / 330; //65-轮胎直径 330-编码器线数 20-编码器获取方波的频率
}

void GetPositionInfo(float *_position, int32 _EncoderDirection1, int32 _EncoderDirection2)
{
    uint16_t _position_enc = 0;
    _position_enc = _EncoderDirection1 > _EncoderDirection2 ? _EncoderDirection1 : _EncoderDirection2;
    if(_EncoderDirection1 > _EncoderDirection2)
    {
        *_position += (float)_position_enc;
    }
    else
        *_position -= (float)_position_enc;
}



void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htimEncoder_Motor);
}


/*
*********************************************************************************************************
*	函 数 名: HAL_TIM_PeriodElapsedCallback
*	功能说明: 定时器5更新中断
*	形    参: htim
*	返 回 值: 无
*********************************************************************************************************
*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    GetSpeedInfo(g_WheelSpeed, s_EncodeValue[0], s_EncodeValue[1], g_EncoderValue.MotorA_EncoderValue.dir);
    GetSpeedInfo(g_WheelSpeed + 1, s_EncodeValue[2], s_EncodeValue[3], g_EncoderValue.MotorB_EncoderValue.dir);
    GetSpeedInfo(g_WheelSpeed + 2, s_EncodeValue[4], s_EncodeValue[5], g_EncoderValue.MotorC_EncoderValue.dir);
    GetSpeedInfo(g_WheelSpeed + 3, s_EncodeValue[6], s_EncodeValue[7], g_EncoderValue.MotorD_EncoderValue.dir);
		GetPositionInfo(g_WheelPositon,s_position_enc_Num[0],s_position_enc_Num[1]);
		GetPositionInfo(g_WheelPositon + 1,s_position_enc_Num[2],s_position_enc_Num[3]);
		GetPositionInfo(g_WheelPositon + 2,s_position_enc_Num[4],s_position_enc_Num[5]);
		GetPositionInfo(g_WheelPositon + 3,s_position_enc_Num[6],s_position_enc_Num[7]);
		
    for(int i = 0; i < 8; i ++)//每次更新中断时，清空：s_EncodeValue[i]
    {
        s_EncodeValue[i] = 0;
				s_position_enc_Num[i] = 0;
    }


    Send_WheelSpeed[0] = -g_WheelSpeed[0];
    Send_WheelSpeed[1] = -g_WheelSpeed[1];
    Send_WheelSpeed[2] = g_WheelSpeed[2];
    Send_WheelSpeed[3] = g_WheelSpeed[3];
		Send_WheelPositon[0] = -g_WheelPositon[0]/330;
		Send_WheelPositon[1] = -g_WheelPositon[1]/330;
		Send_WheelPositon[2] = g_WheelPositon[2]/330;
		Send_WheelPositon[3] = g_WheelPositon[3]/330;
//    KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_1, 0, Send_WheelSpeed[0], _sMotorA_Speed_M), sizeof(kdrobot_channel_1));
//    KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_2, 1, Send_WheelSpeed[1], _sMotorB_Speed_M), sizeof(kdrobot_channel_2));
//    KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_3, 2, Send_WheelSpeed[2], _sMotorC_Speed_M), sizeof(kdrobot_channel_3));
//    KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_4, 3, Send_WheelSpeed[3], _sMotorD_Speed_M), sizeof(kdrobot_channel_4));
    // printf("%d,%d,%d,%d,%d,%d,%d,%d\n", -g_WheelSpeed[0], _sMotorA_Speed_M, -g_WheelSpeed[1], _sMotorB_Speed_M, g_WheelSpeed[2], _sMotorC_Speed_M,  g_WheelSpeed[3], _sMotorD_Speed_M);

    //		printf("%d,%d,%d,%d,%d,%d,%d,%d\n", -g_WheelSpeed[0], -g_WheelSpeed[1], g_WheelSpeed[2],  g_WheelSpeed[3],0,0,0,0);
		KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_1, 0, Send_WheelPositon[0], Pos_target), sizeof(kdrobot_channel_1));
		KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_2, 1, Send_WheelPositon[1], Pos_target), sizeof(kdrobot_channel_2));
		KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_3, 2, Send_WheelPositon[2], Pos_target), sizeof(kdrobot_channel_3));
		KDCOM_SendData(KDCOM_SetData(&kdrobot_channel_4, 3, Send_WheelPositon[3], Pos_target), sizeof(kdrobot_channel_4));
}
/*
*********************************************************************************************************
*	函 数 名: HAL_TIM_IC_CaptureCallback
*	功能说明: TIM_IC回调函数
*	形    参: htim
*	返 回 值: 无
*********************************************************************************************************
*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)/*输入捕获中断回调函数*/
{
    if(htim->Instance == TIM5)/*判断定时器*/
    {

        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)/*判断通道*/
        {
            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 1)
            {
                g_EncoderValue.MotorA_EncoderValue.dir = 1;
                s_EncodeValue[0] ++;
								s_position_enc_Num[0]++;

            }
            else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == 0)
            {
                g_EncoderValue.MotorA_EncoderValue.dir = 0;
                s_EncodeValue[1] ++;
								s_position_enc_Num[1]++;
            }

            //  printf("d: %d\n", g_EncoderValue.MotorA_EncoderValue.dir);
        }

        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {

            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 1)
            {
                g_EncoderValue.MotorB_EncoderValue.dir = 1;
                s_EncodeValue[2] ++;
								s_position_enc_Num[2]++;

            }
            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0)
            {
                g_EncoderValue.MotorB_EncoderValue.dir = 0;
                s_EncodeValue[3] ++;
								s_position_enc_Num[3]++;
            }

            //           g_EncoderValue.LeftEncoderValue1.speed += s_EncodeValue[2] > s_EncodeValue[3] ? s_EncodeValue[2] : s_EncodeValue[3];
            //printf("---%d-%d---\r\n", g_EncoderValue.LeftEncoderValue1.speed,g_EncoderValue.LeftEncoderValue1.dir);
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
        {
            if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == 1)
            {
                g_EncoderValue.MotorC_EncoderValue.dir = 1;
                s_EncodeValue[4] ++;
								s_position_enc_Num[4]++;

            }
            if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4) == 0)
            {
                g_EncoderValue.MotorC_EncoderValue.dir = 0;
                s_EncodeValue[5] ++;
								s_position_enc_Num[5]++;
            }



        }

        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
        {

            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == 1)
            {
                g_EncoderValue.MotorD_EncoderValue.dir = 1;
                s_EncodeValue[6] ++;
								s_position_enc_Num[6]++;

            }
            if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == 0)
            {
                g_EncoderValue.MotorD_EncoderValue.dir = 0;
                s_EncodeValue[7] ++;
								s_position_enc_Num[7]++;
            }

            //           g_EncoderValue.LeftEncoderValue1.speed += s_EncodeValue[6] > s_EncodeValue[7] ? s_EncodeValue[6] : s_EncodeValue[7];
            //printf("---%d-%d---\r\n", g_EncoderValue.LeftEncoderValue1.speed,g_EncoderValue.LeftEncoderValue1.dir);
        }


    }


}

