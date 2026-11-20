#ifndef __BSP_SPI_BUS_H__
#define __BSP_SPI_BUS_H__

#define USE_SPI_POLL

#define SPIx						SPI2

#define SPIx_CLK_ENABLE()			__HAL_RCC_SPI2_CLK_ENABLE()


#define SPIx_FORCE_RESET()			__HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()		__HAL_RCC_SPI2_RELEASE_RESET()

#define SPIx_SCK_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_SCK_GPIO				GPIOB
#define SPIx_SCK_PIN				GPIO_PIN_13


#define SPIx_MISO_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO				GPIOB
#define SPIx_MISO_PIN 				GPIO_PIN_15


#define SPIx_MOSI_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO				GPIOB
#define SPIx_MOSI_PIN 				GPIO_PIN_14

#define SPIx_CS_CLK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_CS_GPIO				GPIOB
#define SPIx_CS_PIN 				GPIO_PIN_12


#define SPIx_IRQn                   SPI1_IRQn
#define SPIx_IRQHandler             SPI1_IRQHandler

/* 重定义下SPI SCK时钟，方便移植 */
#define SPI_BAUDRATEPRESCALER_100M      SPI_BAUDRATEPRESCALER_2			/* 100M */
#define SPI_BAUDRATEPRESCALER_50M       SPI_BAUDRATEPRESCALER_4			/* 50M */
#define SPI_BAUDRATEPRESCALER_12_5M     SPI_BAUDRATEPRESCALER_8			/* 12.5M */
#define SPI_BAUDRATEPRESCALER_6_25M     SPI_BAUDRATEPRESCALER_16		/* 6.25M */
#define SPI_BAUDRATEPRESCALER_3_125M    SPI_BAUDRATEPRESCALER_32		/* 3.125M */
#define SPI_BAUDRATEPRESCALER_1_5625M   SPI_BAUDRATEPRESCALER_64		/* 1.5625M */
#define SPI_BAUDRATEPRESCALER_781_25K   SPI_BAUDRATEPRESCALER_128		/* 781.25K */
#define SPI_BAUDRATEPRESCALER_390_625K  SPI_BAUDRATEPRESCALER_256

#define	SPI_BUFFER_SIZE		(4 * 1024)	

//#define SPI1_CS_1()      SPIx_CS_GPIO->BSRR = SPIx_CS_PIN                        /*CS置1*/
//#define SPI1_CS_0()      SPIx_CS_GPIO->BSRR = ((uint32_t)SPIx_CS_PIN << 16U)     /*CS置0*/


extern uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];
extern uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];
extern uint32_t g_spiLen;

extern uint8_t g_spi_busy;

void bsp_InitSPIBus(void);
void bsp_spiTransfer(void);
void bsp_InitSPIParam(uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity);

void bsp_SpiBusEnter(void);
void bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);


#endif /*__BSP_SPI_BUS_H__*/