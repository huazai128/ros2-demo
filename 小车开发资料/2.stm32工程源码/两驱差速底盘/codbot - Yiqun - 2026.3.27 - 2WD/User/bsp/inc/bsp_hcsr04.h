#ifndef __BSP_HCSR04_H__
#define __BSP_HCSR04_H__

#define SR04_PORT                         	 	 	GPIOB
#define SR04_PIN                         				GPIO_PIN_7
#define TRIG_GPIO_Port 													GPIOB
#define TRIG_Pin 																GPIO_PIN_6

extern uint16_t TIM4CH2_CAPTURE_VAL;
extern uint8_t  TIM4CH2_CAPTURE_STA;

void HCSR_04_Start(void);
float getSR04_fDistance(void);
void HC_SR04_Init(void);
void bsp_InitTim4(void);
float getSR04_fDistance(void);
void Ultrasonic_Follow(float now_dis, float target_dis);
void Avoid_Obstacles(void);

#endif /*__BSP_HCSR04_H__*/


