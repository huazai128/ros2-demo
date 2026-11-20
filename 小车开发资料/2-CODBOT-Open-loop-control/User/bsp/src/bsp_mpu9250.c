#include "bsp.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 

#define MPU9250_INT_PIN    GPIO_PIN_12
#define MPU9250_INT_GPIO   GPIOG

#define MPU9250_INT_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOG_CLK_ENABLE()   

short GYRO_X,GYRO_Y,GYRO_Z,ACCEL_X,ACCEL_Y,ACCEL_Z,MAG_X,MAG_Y,MAG_Z,T_T;		    //陀螺仪X,Y,Z轴原始数据，温度
float pitch,roll,yaw; 	      //俯仰角，偏航角，翻滚角
short aacx,aacy,aacz;	        //加速度传感器原始数据
short gyrox,gyroy,gyroz;      //陀螺仪原始数据 

MPU9250_T g_tMPU9250;
uint8_t bsp_InitMPU9250(void)
{
    uint8_t res = 0;
	MPU9250_WriteByte( MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0x80);
    bsp_DelayMS(100);
    MPU9250_WriteByte( MPU9250_ADDR, MPU_PWR_MGMT1_REG, 0x00);  //唤醒MPU9250
    MPU_SetGyroFsr(3);                                        //陀螺仪传感器,±2000dps
    MPU_SetAccelFsr(0);                                       //加速度传感器,±2g
    MPU_SetRate(50);                                           //设置采样率50Hz
	MPU9250_WriteByte( MPU9250_ADDR, MPU_INT_EN_REG   , 0x00);  //关闭所有中断
	MPU9250_WriteByte( MPU9250_ADDR, MPU_USER_CTRL_REG, 0x00);  //I2C主模式关闭
	MPU9250_WriteByte( MPU9250_ADDR, MPU_FIFO_EN_REG  , 0x00);  //关闭FIFO
    MPU9250_WriteByte( MPU9250_ADDR, MPU_INTBP_CFG_REG, 0x82);  //INT引脚低电平有效，开启bypass模式，可以直接读取磁力计

    res = MPU9250_ReadByte( MPU9250_ADDR, MPU_DEVICE_ID_REG);   // 读取设备ID
    if(res == MPU6500_ID)
    {
        printf("--mpu6500\r\n");
        MPU9250_WriteByte( MPU9250_ADDR, MPU_PWR_MGMT1_REG,0X01);  	//设置CLKSEL,PLL X轴为参考
        MPU9250_WriteByte( MPU9250_ADDR, MPU_PWR_MGMT2_REG,0X00);  	//加速度与陀螺仪都工作
        MPU_SetRate(50);	
    }
    else
    {
        return 1;
    }
    
    res=MPU9250_ReadByte( AK8963_ADDR,MAG_WIA);
    if(res == AK8963_ID)
    {
        printf("--AK8963\r\n");
        MPU9250_WriteByte(AK8963_ADDR,MAG_CNTL1,0X11);		//设置AK8963为单次测量模式
    
    }
    else
    {
        return 1;
    
    }

    return 0;

}

void MPU9250_INITIntGpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; 
    MPU9250_INT_GPIO_CLK_ENABLE();
    
     /* 选择按键1的引脚 */ 
    GPIO_InitStructure.Pin = MPU9250_INT_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    /* 使用上面的结构体初始化按键 */
    HAL_GPIO_Init(MPU9250_INT_GPIO, &GPIO_InitStructure); 
    /* 配置 EXTI 中断源 到key1 引脚、配置中断优先级*/
    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

uint8_t mplstate = 0;
void EXTI15_10_IRQHandler(void)
{
    if(__HAL_GPIO_EXTI_GET_IT(MPU9250_INT_PIN) != RESET) 
	{

     __HAL_GPIO_EXTI_CLEAR_IT(MPU9250_INT_PIN);
        if(mpu_mpl_get_data(&pitch,&roll,&yaw)==0)
        {
//            MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
//            ACCEL_X=aacx/164;ACCEL_Y=aacy/164;ACCEL_Z=aacz/(-164);
//            MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
//            GYRO_X=gyrox/(-16.4);GYRO_Y=gyroy/16.4;GYRO_Z=gyroz/(-16.4);		          
        }
   }        
}


uint8_t MPU9250_WriteByte(uint8_t _deviceAddr, uint8_t _ucRegAddr, uint8_t _ucRegData)
{
    i2c_Start();
    i2c_SendByte((_deviceAddr<<1)|0); //发送器件地址+写命令
    if(i2c_WaitAck())          //等待应答
    {
        i2c_Stop();
        return 1;
    }
    i2c_SendByte(_ucRegAddr);         //写寄存器地址
    i2c_WaitAck();             //等待应答
    i2c_SendByte(_ucRegData);        //发送数据
    if(i2c_WaitAck())          //等待ACK
    {
        i2c_Stop();
        return 1;
    }
    i2c_Stop();
    return 0;
}




uint8_t MPU9250_ReadByte(uint8_t _deviceAddr, uint8_t _ucRegAddr)
{
    uint8_t res;
    i2c_Start();
    i2c_SendByte((_deviceAddr<<1)|0); //发送器件地址+写命令
    i2c_WaitAck();             //等待应答
    i2c_SendByte(_ucRegAddr);         //写寄存器地址
    i2c_WaitAck();             //等待应答
	  i2c_Start();                
    i2c_SendByte((_deviceAddr<<1)|1); //发送器件地址+读命令
    i2c_WaitAck();             //等待应答
    res=i2c_ReadByte(0);		//读数据,发送nACK 
    i2c_NAck();
    i2c_Stop();                 //产生一个停止条件
    return res; 	
}

uint8_t MPU9250_Write_Len(uint8_t _addr, uint8_t _reg, uint8_t _len, uint8_t *_buf)
{
    uint8_t _ack;
    
    i2c_Start();
    i2c_SendByte((_addr << 1) | 0);
    _ack = i2c_WaitAck();
    if(_ack != 0)
    {
        i2c_Stop();

        return 1;
    
    }
    i2c_SendByte(_reg);
    _ack = i2c_WaitAck();
    
    for(int i = 0; i < _len; i ++)
    {
        i2c_SendByte(_buf[i]);
        _ack = i2c_WaitAck();
        if(_ack != 0)
        {
            i2c_Stop();
            return 1;
    
        }
    
    }
    i2c_Stop(); 
   
    return 0;

}

uint8_t MPU9250_Read_Len(uint8_t _addr, uint8_t _reg, uint8_t _len, uint8_t *_buf)
{
    i2c_Start();
    i2c_SendByte((_addr << 1) | 0);
    if(i2c_WaitAck() != 0)
    {
       i2c_Stop();
       return 1;
   
    }
    
    i2c_SendByte(_reg); 
    
    i2c_WaitAck();

    i2c_Start();
    i2c_SendByte((_addr << 1) | 1);
    i2c_WaitAck();

    
    while(_len)
	{
		if(_len==1)
        {
            *_buf = i2c_ReadByte(0);//读数据,发送nACK 
            i2c_NAck();
        }
		else 
        {
            *_buf = i2c_ReadByte(1);
//            i2c_Ack();
        }		//读数据,发送ACK  
		_len--;
		_buf++; 
	} 

    i2c_Stop();
    return 0;
}


//得到温度值
//返回值:温度值(扩大了100倍)
int16_t  MPU_GetTemperature(void)
{
    uint8_t _buf[2]; 
    int16_t raw;
	float temp;
	MPU9250_Read_Len(MPU9250_ADDR, MPU_TEMP_OUTH_REG,2,_buf); 
    raw=((uint16_t)_buf[0] << 8) | _buf[1];  
    temp = 36.53 + ((double)raw)/340;  
    return temp*100;;
}


//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(int16_t *gx,int16_t *gy,int16_t *gz)
{
    uint8_t buf[6] = {0},_res = 0;  
	_res = MPU9250_Read_Len(MPU9250_ADDR,MPU_GYRO_XOUTH_REG, 6, buf);
	if(_res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return _res;;
}

//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(int16_t *ax,int16_t *ay,int16_t *az)
{
    uint8_t buf[6], _res;  
	_res = MPU9250_Read_Len(MPU9250_ADDR,MPU_ACCEL_XOUTH_REG, 6, buf);
	if(_res == 0)
	{
		*ax=((uint16_t)buf[0] << 8) | buf[1];  
		*ay=((uint16_t)buf[2] << 8) | buf[3];  
		*az=((uint16_t)buf[4] << 8) | buf[5];
	} 	
    return _res;;
}
//得到磁力计值(原始值)
//mx,my,mz:磁力计x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Magnetometer(short *mx,short *my,short *mz)
{
    uint8_t buf[6],res;  
	res = MPU9250_Read_Len(AK8963_ADDR,MAG_XOUT_L,6,buf);
	if(res==0)
	{
		*mx=((uint16_t)buf[1]<<8)|buf[0];  
		*my=((uint16_t)buf[3]<<8)|buf[2];  
		*mz=((uint16_t)buf[5]<<8)|buf[4];
	} 	
    MPU9250_WriteByte(AK8963_ADDR,MAG_CNTL1,0X11); //AK8963每次读完以后都需要重新设置为单次测量模式
    return res;;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败
uint8_t MPU_SetGyroFsr(uint8_t fsr)
{
    uint8_t _res = 0;
    _res = MPU9250_WriteByte(MPU9250_ADDR, MPU_GYRO_CFG_REG,fsr<<3);

    return _res;
}


//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_SetAccelFsr(uint8_t fsr)
{
    uint8_t _res = 0;
    _res = MPU9250_WriteByte(MPU9250_ADDR, MPU_ACCEL_CFG_REG,fsr<<3);

    return _res;
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_SetLPF(uint16_t lpf)
{
    uint8_t _data = 0, _res = 0;
	if(lpf>=188)
        _data = 1;
	else if(lpf>=98)
        _data = 2;
	else if(lpf>=42)
        _data = 3;
	else if(lpf>=20)
        _data = 4;
	else if(lpf>=10)
        _data = 5;
	else 
        _data = 6;
    
    _res = MPU9250_WriteByte(MPU9250_ADDR, MPU_CFG_REG, _data);
    return _res;
}

//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_SetRate(uint16_t _rate)
{
	uint8_t _data;
	if(_rate > 1000)
        _rate = 1000;
	if(_rate < 4)
        _rate = 4;
	_data = 1000 / _rate -1;
	_data = MPU9250_WriteByte(MPU9250_ADDR, MPU_SAMPLE_RATE_REG, _data);	//设置数字低通滤波器
 	return MPU_SetLPF(_rate / 2);	//自动设置LPF为采样率的一半
}

//void MPU9250_ReadData(void)
//{
//	uint8_t ucReadBuf[14];
//	uint8_t i;
//	uint8_t ack;

//#if 1 /* 连续读 */
//	i2c_Start();                  			/* 总线开始信号 */
//	i2c_SendByte(MPU9250_SLAVE_ADDRESS);	/* 发送设备地址+写信号 */
//	ack = i2c_WaitAck();
//	if (ack != 0)
//	{
//		i2c_Stop(); 
//		return;
//	}
//	i2c_SendByte(ACCEL_XOUT_H);     		/* 发送存储单元地址  */
//	ack = i2c_WaitAck();
//	if (ack != 0)
//	{
//		i2c_Stop(); 
//		return;
//	}

//	i2c_Start();                  			/* 总线开始信号 */

//	i2c_SendByte(MPU9250_SLAVE_ADDRESS + 1); /* 发送设备地址+读信号 */
//	ack = i2c_WaitAck();
//	if (ack != 0)
//	{
//		i2c_Stop(); 
//		return;
//	}

//	for (i = 0; i < 13; i++)
//	{
//		ucReadBuf[i] = i2c_ReadByte();       			/* 读出寄存器数据 */
//		i2c_Ack();
//	}

//	/* 读最后一个字节，时给 NAck */
//	ucReadBuf[13] = i2c_ReadByte();
//	i2c_NAck();

//	i2c_Stop();                  			/* 总线停止信号 */

//#else	/* 单字节读 */
//	for (i = 0 ; i < 14; i++)
//	{
//		ucReadBuf[i] = MPU6050_ReadByte(ACCEL_XOUT_H + i);
//	}
//#endif
//    MPU_Read_Len(MPU9250_SLAVE_ADDRESS, ACCEL_XOUT_H, 14, ucReadBuf);

//	/* 将读出的数据保存到全局结构体变量 */
//	g_tMPU9250.Accel_X = (ucReadBuf[0] << 8) + ucReadBuf[1];
//	g_tMPU9250.Accel_Y = (ucReadBuf[2] << 8) + ucReadBuf[3];
//	g_tMPU9250.Accel_Z = (ucReadBuf[4] << 8) + ucReadBuf[5];

//	g_tMPU9250.Temp = (int16_t)((ucReadBuf[6] << 8) + ucReadBuf[7]);

//	g_tMPU9250.GYRO_X = (ucReadBuf[8] << 8) + ucReadBuf[9];
//	g_tMPU9250.GYRO_Y = (ucReadBuf[10] << 8) + ucReadBuf[11];
//	g_tMPU9250.GYRO_Z = (ucReadBuf[12] << 8) + ucReadBuf[13];
//    
//    printf("ACCEL: %d, %d, %d, GYRO: %d, %d, %d\r\n", g_tMPU9250.Accel_X, g_tMPU9250.Accel_Y, g_tMPU9250.Accel_Z, g_tMPU9250.GYRO_X, g_tMPU9250.GYRO_Y, g_tMPU9250.GYRO_Z);

//}


