#ifndef __BSP_CCD_H__
#define __BSP_CCD_H__

#define TSL1401_CCD_AO_Port                     GPIOC
#define TSL1401_CCD_AO_Pin                      GPIO_PIN_5
#define TSL1401_CCD_SI_Port 										GPIOB
#define TSL1401_CCD_SI_Pin 											GPIO_PIN_11
#define TSL1401_CCD_CLK_Port 										GPIOA
#define TSL1401_CCD_CLK_Pin 										GPIO_PIN_11

void bsp_Init_CCD_Adc(void);
void bsp_InitCCD(void);
uint16_t getCCDAdcvalue(uint32_t ch);
void Read_CCD_Val(void);
extern uint16_t CCD_Val[128];
void  Find_CCD_MidVal(void);
extern uint8_t CCD_MidVal, CCD_Threshold;
#endif /*__BSP_ENCODER_H__*/


