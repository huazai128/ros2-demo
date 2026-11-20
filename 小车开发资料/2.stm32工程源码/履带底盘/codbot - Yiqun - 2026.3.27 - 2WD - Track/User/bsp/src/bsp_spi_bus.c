#include "bsp.h"



enum {
    TRANSFER_WAIT,
	TRANSFER_COMPLETE,
	TRANSFER_ERROR
};


static SPI_HandleTypeDef hspi = {0};


/* transfer state */
__IO uint32_t wTransferState = TRANSFER_WAIT;
//const uint8_t g_spiTxBuf[] = "****SPI - Two Boards communication based on DMA **** SPI Message ********* SPI Message *********";
uint8_t g_spiTxBuf[SPI_BUFFER_SIZE];

uint8_t g_spiRxBuf[SPI_BUFFER_SIZE];	/* 必须32字节对齐 */
uint32_t g_spiLen;		/* 收发的数据长度 */



/* 备份SPI几个关键传输参数，波特率，相位，极性. 如果不同外设切换，需要重新Init SPI参数 */
static uint32_t s_BaudRatePrescaler;
static uint32_t s_CLKPhase;
static uint32_t s_CLKPolarity;

uint8_t g_spi_busy;



void bsp_InitSPIParam(uint32_t _BaudRatePrescaler, uint32_t _CLKPhase, uint32_t _CLKPolarity)

{
    /* 提高执行效率，只有在SPI硬件参数发生变化时，才执行HAL_Init */
	if (s_BaudRatePrescaler == _BaudRatePrescaler && s_CLKPhase == _CLKPhase && s_CLKPolarity == _CLKPolarity)
	{		
		return;
	}
    
    s_BaudRatePrescaler = _BaudRatePrescaler;	
	s_CLKPhase = _CLKPhase;
	s_CLKPolarity = _CLKPolarity;
	
	/*##-1- Configure the SPI peripheral #######################################*/
	/* Set the SPI parameters */
	hspi.Instance               = SPIx;
	hspi.Init.BaudRatePrescaler = _BaudRatePrescaler;
	hspi.Init.Direction         = SPI_DIRECTION_2LINES;
	hspi.Init.CLKPhase          = _CLKPhase;
	hspi.Init.CLKPolarity       = _CLKPolarity;
	hspi.Init.DataSize          = SPI_DATASIZE_8BIT;
	hspi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	hspi.Init.TIMode            = SPI_TIMODE_DISABLE;
	hspi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	hspi.Init.CRCPolynomial     = 7;
	hspi.Init.NSS               = SPI_NSS_SOFT;
	hspi.Init.Mode 			 = SPI_MODE_MASTER;

	if (HAL_SPI_Init(&hspi) != HAL_OK)
	{
		/* Initialization Error */
		Error_Handler(__FILE__, __LINE__);
	}	


}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIBus1
*	功能说明: 配置SPI总线。 只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/

void bsp_InitSPIBus(void)
{
    g_spi_busy = 0;
    bsp_InitSPIParam(SPI_BAUDRATEPRESCALER_8, SPI_PHASE_1EDGE, SPI_POLARITY_LOW);

}


void HAL_SPI_MspInit(SPI_HandleTypeDef *_hspi)
{
	/* 配置 SPI总线GPIO : SCK MOSI MISO */
	{
		GPIO_InitTypeDef  GPIO_InitStruct;
			
		/*##-1- Enable peripherals and GPIO Clocks #################################*/
		SPIx_SCK_CLK_ENABLE();
		SPIx_MISO_CLK_ENABLE();
		SPIx_MOSI_CLK_ENABLE();
        SPIx_CS_CLK_ENABLE();
		/* Enable SPI1 clock */
		SPIx_CLK_ENABLE();

		/*##-2- Configure peripheral GPIO ##########################################*/  
		/* SPI SCK GPIO pin configuration  */
		GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
		GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull      = GPIO_PULLDOWN;		/* 下拉 */
		GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_MEDIUM;
		HAL_GPIO_Init(SPIx_SCK_GPIO, &GPIO_InitStruct);

		/* SPI MISO GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MISO_PIN;
		HAL_GPIO_Init(SPIx_MISO_GPIO, &GPIO_InitStruct);

		/* SPI MOSI GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
		HAL_GPIO_Init(SPIx_MOSI_GPIO, &GPIO_InitStruct);
        
        /* SPI CS GPIO pin configuration  */
		GPIO_InitStruct.Pin = SPIx_CS_PIN;
		HAL_GPIO_Init(SPIx_CS_GPIO, &GPIO_InitStruct);
	}
}


/*
*********************************************************************************************************
*	函 数 名: bsp_spiTransfer
*	功能说明: 向SPI总线发送1个或多个字节
*	形    参:  g_spi_TxBuf : 发送的数据
*		      g_spi_RxBuf  ：收发的数据长度
*			  _TxRxLen ：接收的数据缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_spiTransfer(void)
{
	if (g_spiLen > SPI_BUFFER_SIZE)
	{
		return;
	}
	
#ifdef USE_SPI_DMA
	wTransferState == TRANSFER_WAIT;
	
	while (hspi.State != HAL_SPI_STATE_READY);

	if(HAL_SPI_TransmitReceive_DMA(&hspi, (uint8_t*)g_spiTxBuf, (uint8_t *)g_spiRxBuf, g_spiLen) != HAL_OK)	
	{
		Error_Handler(__FILE__, __LINE__);
	}
	
	while (wTransferState == TRANSFER_WAIT)
	{
		;
	}
	
	/* Invalidate cache prior to access by CPU */
	SCB_InvalidateDCache_by_Addr ((uint32_t *)g_spiRxBuf, SPI_BUFFER_SIZE);
#endif
	
#ifdef USE_SPI_INT
	wTransferState = TRANSFER_WAIT;
	
	while (hspi.State != HAL_SPI_STATE_READY);

	if(HAL_SPI_TransmitReceive_IT(&hspi, (uint8_t*)g_spiTxBuf, (uint8_t *)g_spiRxBuf, g_spiLen) != HAL_OK)	
	{
		Error_Handler(__FILE__, __LINE__);
	}
	
	while (wTransferState == TRANSFER_WAIT)
	{
		;
	}
#endif
	
#ifdef USE_SPI_POLL
	if(HAL_SPI_TransmitReceive(&hspi, (uint8_t*)g_spiTxBuf, (uint8_t *)g_spiRxBuf, g_spiLen, 1000000) != HAL_OK)	
	{
		Error_Handler(__FILE__, __LINE__);
	}	
#endif
}


/**
  * @brief  TxRx Transfer completed callback.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report end of DMA TxRx transfer, and 
  *         you can add your own implementation. 
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	wTransferState = TRANSFER_COMPLETE;
}

/**
  * @brief  SPI error callbacks.
  * @param  hspi: SPI handle
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	wTransferState = TRANSFER_ERROR;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusEnter
*	功能说明: 占用SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusEnter(void)
{
	g_spi_busy = 1;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusExit
*	功能说明: 释放占用的SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusExit(void)
{
	g_spi_busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusBusy
*	功能说明: 判断SPI总线忙。方法是检测其他SPI芯片的片选信号是否为1
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
uint8_t bsp_SpiBusBusy(void)
{
	return g_spi_busy;
}


#ifdef USE_SPI_INT
	/**
	  * @brief  This function handles SPIx interrupt request.
	  * @param  None
	  * @retval None
	  */
	void SPIx_IRQHandler(void)
	{
		HAL_SPI_IRQHandler(&hspi);
	}	
#endif

#ifdef USE_SPI_DMA
	/**
	  * @brief  This function handles DMA Rx interrupt request.
	  * @param  None
	  * @retval None
	  */
	void SPIx_DMA_RX_IRQHandler(void)
	{
		HAL_DMA_IRQHandler(hspi.hdmarx);
	}

	/**
	  * @brief  This function handles DMA Tx interrupt request.
	  * @param  None
	  * @retval None
	  */
	void SPIx_DMA_TX_IRQHandler(void)
	{
		HAL_DMA_IRQHandler(hspi.hdmatx);
	}
	
	/**
	  * @brief  This function handles SPIx interrupt request.
	  * @param  None
	  * @retval None
	  */
	void SPIx_IRQHandler(void)
	{
		HAL_SPI_IRQHandler(&hspi);
	}	
#endif