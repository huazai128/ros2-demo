#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdio.h>

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
#define CANx                            CAN1
#define CANx_CLK_ENABLE()               __HAL_RCC_CAN1_CLK_ENABLE()
#define CANx_FORCE_RESET()              __HAL_RCC_CAN1_FORCE_RESET()
#define CANx_RELEASE_RESET()            __HAL_RCC_CAN1_RELEASE_RESET()

#define CANx_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOA_CLK_ENABLE()
#define CANx_GPIO_PORT                 GPIOA
#define CANx_TX_PIN                    GPIO_PIN_12
#define CANx_RX_PIN                    GPIO_PIN_11

/* Definition for AFIO Remap */
#define CANx_AFIO_REMAP_CLK_ENABLE()   __HAL_RCC_AFIO_CLK_ENABLE()
#define CANx_AFIO_REMAP_RX_TX_PIN()    __HAL_AFIO_REMAP_CAN1_1()

#define CANx_RX_IRQn                   USB_LP_CAN1_RX0_IRQn
#define CANx_RX_IRQHandler             USB_LP_CAN1_RX0_IRQHandler

/* 扩展变量 ------------------------------------------------------------------*/
extern CAN_HandleTypeDef hCAN;

/* 函数声明 ------------------------------------------------------------------*/
void bsp_InitCan(void);
void CanTransmit(uint16_t _id, uint8_t *_buf, uint8_t _num);
#endif /* __BSP_CAN_H__ */