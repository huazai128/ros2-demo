#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__


typedef struct
{
    uint8_t  dir;
    uint16_t speed;
   
}CarSpeedInfo_t;

typedef struct {

    CarSpeedInfo_t MotorA_EncoderValue;
    CarSpeedInfo_t MotorB_EncoderValue;
    CarSpeedInfo_t MotorC_EncoderValue;
    CarSpeedInfo_t MotorD_EncoderValue;

}Encoder_t;



void bsp_InitEncoder(void);
void GetEncoderValue(Encoder_t *_encoder);
void GetSpeedInfo(int16_t *_speed, uint16_t _EncoderDirection1,uint16_t _EncoderDirection2, uint16_t _FeedbackDir);

extern Encoder_t g_EncoderValue;

extern int16_t g_WheelSpeed[4];
extern float g_WheelPositon[4];
extern int16_t Send_WheelSpeed[4];



#endif /*__BSP_ENCODER_H__*/


