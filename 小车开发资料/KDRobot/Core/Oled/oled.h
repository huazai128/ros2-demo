#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"	
#include "spi.h"
#include "SOLGUI_Hardware.h"
//-----------------OLED端口定义---------------- 

#define OLED_CS_EN 		0

//D0 - CLK
//D1 - DAT

// #define LED_Pin GPIO_PIN_13
// #define LED_GPIO_Port GPIOC

#define OLED_DC_Pin OLED_DC__Pin
#define OLED_DC_GPIO_Port OLED_DC__GPIO_Port

#define OLED_RES_Pin OLED_RES__Pin
#define OLED_RES_GPIO_Port OLED_RES__GPIO_Port

#if OLED_CS_EN == 1
#define OLED_CS_Pin GPIO_PIN_2
#define OLED_CS_GPIO_Port GPIOB
#endif

#define OLED_RES_Clr() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET)//RES
#define OLED_RES_Set() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET)

#define OLED_DC_Clr()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET)//DC
#define OLED_DC_Set()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET)

////////////////////////////

#if OLED_CS_EN == 1
#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_SET)
#endif

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_Init(void);
void OLED_Refresh(uint8_t SOLGUI_GRAM[SCREEN_X_WIDTH][SCREEN_Y_PAGE]);
void OLED_Clear(void);
void OLED_WR_Byte(uint8_t dat, uint8_t mode);

#endif

