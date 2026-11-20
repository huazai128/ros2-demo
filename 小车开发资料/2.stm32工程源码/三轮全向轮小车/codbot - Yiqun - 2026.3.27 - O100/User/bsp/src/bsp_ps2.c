#include "bsp.h"


uint8_t g_Ps2Data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t g_Ps2Cmd[2] = {0x01, 0x42};	//开始命令。请求数据

static uint16_t s_Ps2Key = 0;

static uint16_t MASK[] =
{
    PSB_SELECT,
    PSB_L3,
    PSB_R3,
    PSB_START,
    PSB_PAD_UP,
    PSB_PAD_RIGHT,
    PSB_PAD_DOWN,
    PSB_PAD_LEFT,
    PSB_L2,
    PSB_R2,
    PSB_L1,
    PSB_R1,
    PSB_GREEN,
    PSB_RED,
    PSB_BLUE,
    PSB_PINK
};	//按键值与按键明


void bsp_InitPs2(void)
{

    __HAL_RCC_GPIOB_CLK_ENABLE();


    GPIO_InitTypeDef  GPIO_InitStruct;
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;		/* 下拉 */
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SPIx_SCK_GPIO, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin     = SPIx_MOSI_PIN;
    HAL_GPIO_Init(SPIx_MOSI_GPIO, &GPIO_InitStruct);

    /*SPI CS GPIO pin configuration  */
    GPIO_InitStruct.Pin      = SPIx_CS_PIN;
    HAL_GPIO_Init(SPIx_CS_GPIO, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Mode     = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin      = SPIx_MISO_PIN;
    HAL_GPIO_Init(SPIx_MISO_GPIO, &GPIO_InitStruct);

}



void ps2Cmd(uint8_t _data)
{
    volatile uint16_t ref = 0x01;
    g_Ps2Data[1] = 0x00;

    for(ref = 0x01; ref < 0x0100; ref <<= 1)
    {
        if(ref & _data)
        {
            SPI1_MOSI_1();
        }
        else
        {
            SPI1_MOSI_0();
        }

        SPI1_CLK_1();

        bsp_DelayUS(50);

        SPI1_CLK_0();

        bsp_DelayUS(50);

        SPI1_CLK_1();

        if(SPI1_MISO())
        {
            g_Ps2Data[1] = ref | g_Ps2Data[1];
        }
    }
    bsp_DelayUS(16);

}

void ps2GetData(void)
{
    volatile uint8_t  byte = 0;
    volatile uint16_t ref = 0x01;

    SPI1_CS_0();

    ps2Cmd(g_Ps2Cmd[0]);
    ps2Cmd(g_Ps2Cmd[1]);

    for(byte = 2; byte < 9; byte++)    //???????????
    {
        for(ref = 0x01; ref < 0x100; ref <<= 1)
        {
            SPI1_CLK_1();
            SPI1_CLK_0();
            bsp_DelayUS(50);
            SPI1_CLK_1();
            if(SPI1_MISO())
                g_Ps2Data[byte] = ref | g_Ps2Data[byte];
        }

        bsp_DelayUS(16);
    }

    SPI1_CS_1();

}

//清除数据缓冲区
void ps2ClearData(void)
{
    uint8_t i;
    for(i = 0; i < 9; i++)
        g_Ps2Data[i] = 0x00;

}

uint8_t  ps2DataKey(void)
{
    uint8_t _index;

    //    for(int i = 0; i < 9; i ++)
    //    {
    //        printf("%x ", g_Ps2Data[i]);
    //    }
    //    printf("\r\n");
    s_Ps2Key = (g_Ps2Data[4] << 8) | g_Ps2Data[3];

    for(_index = 0; _index < 16; _index++)
    {
        if((s_Ps2Key & (1 << (MASK[_index] - 1))) == 0)
            return _index + 1;
    }



    return 0;
}


uint8_t ps2CheckMode(void)
{
    uint8_t x = g_Ps2Data[1];


    if(x == 0x73) /*红灯模式*/
        return 1;
    else          /*绿灯模式*/
        return 0;

}

uint8_t ps2DisLinkStatus(void)
{
    if(g_Ps2Data[6] == 0x80 & g_Ps2Data[7] == 0x80 & g_Ps2Data[8] == 0x80) /*断联模式(红灯模式下断联)*/
        return 1;
    else
        return 0;

}

void ps2GetAnalogValue(Ps2Pad_t *_leftPs2Pad, Ps2Pad_t *_rightPs2Pad, Ps2Key_t *_Ps2Key)
{

    _leftPs2Pad->Ps2PadXValue  = g_Ps2Data[7];
    _leftPs2Pad->Ps2PadYValue  = g_Ps2Data[8];
    _rightPs2Pad->Ps2PadXValue = g_Ps2Data[5];
    _rightPs2Pad->Ps2PadYValue = g_Ps2Data[6];
		if(g_Ps2Data[4] == 0xfb)
			_Ps2Key->Ps2KeyL1 = 1;
		else _Ps2Key->Ps2KeyL1 = 0;
		
		if(g_Ps2Data[4] == 0xfe)
			_Ps2Key->Ps2KeyL2 = 1;
		else _Ps2Key->Ps2KeyL2 = 0;
		
		if(g_Ps2Data[3] == 0xef)
			_Ps2Key->Ps2KeyLeftUp = 1;
		else _Ps2Key->Ps2KeyLeftUp = 0;
		
		if(g_Ps2Data[3] == 0xbf)
			_Ps2Key->Ps2KeyLeftDown = 1;
		else _Ps2Key->Ps2KeyLeftDown = 0;
		
		if(g_Ps2Data[3] == 0x7f)
			_Ps2Key->Ps2KeyLeftLeft = 1;
		else _Ps2Key->Ps2KeyLeftLeft = 0;
		
		if(g_Ps2Data[3] == 0xdf)
			_Ps2Key->Ps2KeyLeftRight = 1;
		else _Ps2Key->Ps2KeyLeftRight = 0;
		
		
		if(g_Ps2Data[4] == 0xf7)
			_Ps2Key->Ps2KeyR1 = 1;
		else _Ps2Key->Ps2KeyR1 = 0;
		
		if(g_Ps2Data[4] == 0xfd)
			_Ps2Key->Ps2KeyR2 = 1;
		else _Ps2Key->Ps2KeyR2 = 0;
		
		if(g_Ps2Data[4] == 0xef)
			_Ps2Key->Ps2KeyRightUp = 1;
		else _Ps2Key->Ps2KeyRightUp = 0;
		
		if(g_Ps2Data[4] == 0xbf)
			_Ps2Key->Ps2KeyRightDown = 1;
		else _Ps2Key->Ps2KeyRightDown = 0;
		
		if(g_Ps2Data[4] == 0x7f)
			_Ps2Key->Ps2KeyRightLeft = 1;
		else _Ps2Key->Ps2KeyRightLeft = 0;
		
		if(g_Ps2Data[4] == 0xdf)
			_Ps2Key->Ps2KeyRightRight = 1;
		else _Ps2Key->Ps2KeyRightRight = 0;
}