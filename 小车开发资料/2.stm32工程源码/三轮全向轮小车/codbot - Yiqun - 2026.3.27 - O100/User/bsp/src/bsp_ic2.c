#include "bsp.h"

#define I2C_SCL_PIN    GPIO_PIN_8
#define I2C_SCL_GPIO   GPIOB

#define I2C_SDA_PIN    GPIO_PIN_9
#define I2C_SDA_GPIO   GPIOB

#define ALL_I2C_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2C_SCL_1()                  HAL_GPIO_WritePin(I2C_SCL_GPIO, I2C_SCL_PIN, GPIO_PIN_SET);
#define I2C_SCL_0()                  HAL_GPIO_WritePin(I2C_SCL_GPIO, I2C_SCL_PIN, GPIO_PIN_RESET);
#define I2C_SDA_1()                  HAL_GPIO_WritePin(I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_PIN_SET);
#define I2C_SDA_0()                  HAL_GPIO_WritePin(I2C_SDA_GPIO, I2C_SDA_PIN, GPIO_PIN_RESET);

#define I2C_SDA_READ()  			HAL_GPIO_ReadPin(I2C_SDA_GPIO, I2C_SDA_PIN)	/* ??SDA?????? */
#define I2C_SCL_READ()  			HAL_GPIO_ReadPin(I2C_SCL_GPIO, I2C_SCL_PIN)	/* ??SCL?????? */

#define MPU_SDA_IN()  {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=8<<4;}
#define MPU_SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=3<<4;}


void bsp_InitI2C(void)
{
	GPIO_InitTypeDef gpio_init;

	/* ??1??????GPIO??? */
	ALL_I2C_GPIO_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_OD;	/* ????????? */
	gpio_init.Pull = GPIO_NOPULL;			/* ?????????ÙM??? */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;  /* GPIO????? */
	
	gpio_init.Pin = I2C_SCL_PIN;	
	HAL_GPIO_Init(I2C_SCL_GPIO, &gpio_init);	
	
	gpio_init.Pin = I2C_SDA_PIN;	
	HAL_GPIO_Init(I2C_SDA_GPIO, &gpio_init);	

	/* ??????????, ??¦ËI2C????????????õô???????? */
	i2c_Stop();


}

static void i2c_Delay(void)
{
	bsp_DelayUS(2);
}

void i2c_Start(void)
{
    MPU_SDA_OUT()
	I2C_SDA_1();
	I2C_SCL_1();
	i2c_Delay();
	I2C_SDA_0();
	i2c_Delay();

	I2C_SCL_0();

}

void i2c_Stop(void)
{
    MPU_SDA_OUT();
	I2C_SCL_0();
    I2C_SDA_0();
	i2c_Delay();
    I2C_SCL_1();
	I2C_SDA_1();
	i2c_Delay();
}
	
void i2c_SendByte(uint8_t _ucByte)
{
	uint8_t i;
	
	for(i = 0; i < 8; i ++)
	{
		if(_ucByte & 0x80)
		{
			I2C_SDA_1();
		}
		else
		{
		    I2C_SDA_0();
		
		}
        _ucByte <<= 1;
		I2C_SCL_1();
		i2c_Delay();
		I2C_SCL_0();
		if(i == 7)
		{
			I2C_SDA_1();
		}
		
		
		i2c_Delay();
	}

}

uint8_t i2c_ReadByte(uint8_t ack)
{
	uint8_t i = 0;
	uint8_t _value = 0;
    MPU_SDA_IN();
	for(i = 0; i < 8; i ++)
	{
		_value <<= 1;
		I2C_SCL_0();
		i2c_Delay();
        I2C_SCL_1();
		if(I2C_SDA_READ())
		{
			_value ++;
		}
		i2c_Delay();
	}
	
     if (!ack)
        i2c_NAck();//·¢ËÍnACK
    else
        i2c_Ack(); //·¢ËÍACK  
	return _value;

}

/*
*********************************************************************************************************
*	?? ?? ??: i2c_WaitAck
*	???????: CPU??????????????????????ACK??????
*	??    ??:  ??
*	?? ?? ?: ????0?????????1????????????
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
	uint8_t  ucErrTime=0;
    MPU_SDA_IN();
	I2C_SDA_1();	/* CPU???SDA???? */
	i2c_Delay();
	I2C_SCL_1();	/* CPU????SCL = 1, ???????????ACK??? */
	i2c_Delay();
   
    while(I2C_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			i2c_Stop();
			return 1;
		}
	}
	I2C_SCL_0();
	return 0;
}


/*
*********************************************************************************************************
*	?? ?? ??: i2c_Ack
*	???????: CPU???????ACK???
*	??    ??:  ??
*	?? ?? ?: ??
*********************************************************************************************************
*/
void i2c_Ack(void)
{
	I2C_SCL_0();	/* CPU????SDA = 0 */
    MPU_SDA_OUT();
    I2C_SDA_0();
	i2c_Delay();
	I2C_SCL_1();	/* CPU????1????? */
	i2c_Delay();
	I2C_SCL_0();

}

/*
*********************************************************************************************************
*	?? ?? ??: i2c_NAck
*	???????: CPU????1??NACK???
*	??    ??:  ??
*	?? ?? ?: ??
*********************************************************************************************************
*/
void i2c_NAck(void)
{
    I2C_SCL_0();
    MPU_SDA_OUT();
	I2C_SDA_1();	/* CPU????SDA = 1 */
	i2c_Delay();
	I2C_SCL_1();	/* CPU????1????? */
	i2c_Delay();
	I2C_SCL_0();
}

/*
*********************************************************************************************************
*	?? ?? ??: i2c_CheckDevice
*	???????: ???I2C?????õô??CPU?????õô???????????õô??????§Ø???õô??????
*	??    ??:  _Address???õô??I2C??????
*	?? ?? ?: ????? 0 ???????? ????1???¦Ä???
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;

	if (I2C_SDA_READ() && I2C_SCL_READ())
	{
       
		i2c_Start();		/* ??????????? */

		/* ?????õô???+??§Õ????bit??0 = w?? 1 = r) bit7 ??? */
		i2c_SendByte(_Address | I2C_WR);
		ucAck = i2c_WaitAck();	/* ????õô??ACK??? */

		i2c_Stop();			/* ????????? */
       
		return ucAck;
	}
    
	return 1;	/* I2C?????? */
}




