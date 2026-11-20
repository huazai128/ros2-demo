#ifndef __PS2_H__
#define __PS2_H__

#include "stdint.h"
//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2          9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_GREEN       13
#define PSB_RED         14
#define PSB_BLUE        15
#define PSB_PINK        16

#define PSB_TRIANGLE    13
#define PSB_CIRCLE      14
#define PSB_CROSS       15
#define PSB_SQUARE      16

#define SPI1_CS_1()      SPIx_CS_GPIO->BSRR = SPIx_CS_PIN                        /*CSÖÃ1*/
#define SPI1_CS_0()      SPIx_CS_GPIO->BSRR = ((uint32_t)SPIx_CS_PIN << 16U)     /*CSÖÃ0*/

#define SPI1_CLK_1()      SPIx_SCK_GPIO->BSRR = SPIx_SCK_PIN                        /*CSÖÃ1*/
#define SPI1_CLK_0()      SPIx_SCK_GPIO->BSRR = ((uint32_t)SPIx_SCK_PIN << 16U)     /*CSÖÃ0*/

#define SPI1_MOSI_1()     SPIx_MOSI_GPIO->BSRR = SPIx_MOSI_PIN                        /*CSÖÃ1*/
#define SPI1_MOSI_0()     SPIx_MOSI_GPIO->BSRR = ((uint32_t)SPIx_MOSI_PIN << 16U)     /*CSÖÃ0*/

#define SPI1_MISO()       HAL_GPIO_ReadPin(SPIx_MISO_GPIO ,SPIx_MISO_PIN)


typedef struct {

    uint8_t Ps2PadXValue;
    uint8_t Ps2PadYValue;

}Ps2Pad_t;
typedef struct
{
	uint8_t Ps2KeyL1;
	uint8_t Ps2KeyL2;
	uint8_t Ps2KeyLeftUp;
	uint8_t Ps2KeyLeftDown;
	uint8_t Ps2KeyLeftLeft;
	uint8_t Ps2KeyLeftRight;
	uint8_t Ps2KeyR1;
	uint8_t Ps2KeyR2;
	uint8_t Ps2KeyRightUp;
	uint8_t Ps2KeyRightDown;
	uint8_t Ps2KeyRightLeft;
	uint8_t Ps2KeyRightRight;
}Ps2Key_t;
void bsp_InitPs2(void);

void ps2GetData(void);
void ps2ClearData(void);
void ps2Cmd(uint8_t _data);

uint8_t  ps2DataKey(void);
uint8_t  ps2CheckMode(void);
void ps2GetAnalogValue(Ps2Pad_t *_leftPs2Pad, Ps2Pad_t *_rightPs2Pad, Ps2Key_t *_Ps2Key);
uint8_t  ps2DisLinkStatus(void);
#endif /*__PS2_H__*/