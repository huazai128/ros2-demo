#ifndef __BSP_TIMER_H__

#define __BSP_TIMER_H__

#define MOTOAB_DS()  GPIOA->BSRR = ((uint32_t)GPIO_PIN_15 << 16U)
#define MOTOAB_EN()  GPIOA->BSRR =  GPIO_PIN_15 


typedef struct{

    uint16_t TimCCR1;
    uint16_t TimCCR2;
    uint16_t TimCCR3;
    uint16_t TimCCR4;

    void (*TimeChannellHandle)();
    void (*TimeChannel2Handle)();
    void (*TimeChannel3Handle)();
    void (*TimeChannel4Handle)();

}TimServo_t;


extern TIM_HandleTypeDef    htim4Motor;
extern TIM_HandleTypeDef    htim3Motor;
extern TIM_HandleTypeDef    htimServo;
extern TIM_HandleTypeDef    htimServo1;
extern TIM_HandleTypeDef    htimSR04;
extern TIM_HandleTypeDef    htimEncoder;
extern TIM_HandleTypeDef    htimEncoder_Motor;
extern TimServo_t           g_Servo;

void bsp_InitTim2(void);
void bsp_InitTim3(void);
void bsp_InitTim8(void);
void bsp_InitTim5(void);
void bsp_InitTim1(void);


#endif /*__BSP_TIMER_H__*/