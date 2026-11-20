/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "oled.h"
#include "SOLGUI_Include.h"
#include "comm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t txbuf[80], rxbuf[40], showbuf[40];
uint8_t startflag = 0, stopflag = 0, channel = 0, sw = 0;
KDRobot *kdrobot = (KDRobot*)rxbuf;
float chxp[] = {1,2,3,4};
float chxi[] = {2,3,4,5};
float chxd[] = {3,4,5,6};
float chxt[] = {4,5,6,7};
float chxa[] = {5,6,7,8};

float *pchxp[] = {chxp,chxp+1,chxp+2,chxp+3};
float *pchxi[] = {chxi,chxi+1,chxi+2,chxi+3};
float *pchxd[] = {chxd,chxd+1,chxd+2,chxd+3};
float *pchxt[] = {chxt,chxt+1,chxt+2,chxt+3};
float *pchxa[] = {chxa,chxa+1,chxa+2,chxa+3};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  SOLGUI_Init();
  HAL_UART_Transmit_DMA(&huart1, (uint8_t*)"Hello KDRobot", 13);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxbuf, 40);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(sw)
    {
      if(startflag)
      {
        KDRobot kd;
        static float x = 0;
        for(int idx = 0; idx < 4; idx++)
        {
          HAL_UART_Transmit(&huart1, KDCOM_SetData(&kd, (uint8)idx, 100.0 * sin(x + idx) * cos(x + idx + 0.31) + 50.0 * sin(0.1 * x) + idx * 100.0, chxa[0]), sizeof(KDRobot), 10);
        }
        HAL_Delay(10);
        x += 0.05;
      }
      if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0))
      {
        sw = 0;
      }
    }
    else
    {
      if(startflag)
      {
        static float x = 0;
        sprintf((char *)txbuf, "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", 100.0*sin(x), -100.0, 100.0*sin(x+1.0), 100.0, 200.0*sin(x+2.0), -200.0, 50.0*sin(x+3.0), 200.0);
        HAL_UART_Transmit(&huart1, txbuf, strlen((const char*)txbuf), 10);
        HAL_Delay(50);
        x += 0.2;
      }
      if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1))
      {
        sw = 1;
      }
    }
    if(stopflag)
    {
      startflag = 0;
      stopflag = 0;
    }
    sprintf((char*)showbuf, "P:%.2f %.2f %.2f %.2f", chxp[0],chxp[1],chxp[2],chxp[3]);
    SOLGUI_printf(0, 0, F6X8, showbuf);
    sprintf((char*)showbuf, "I:%.2f %.2f %.2f %.2f", chxi[0],chxi[1],chxi[2],chxi[3]);
    SOLGUI_printf(0, 8, F6X8, showbuf);
    sprintf((char*)showbuf, "D:%.2f %.2f %.2f %.2f", chxd[0],chxd[1],chxd[2],chxd[3]);
    SOLGUI_printf(0, 16, F6X8, showbuf);
    sprintf((char*)showbuf, "T:%.2f %.2f %.2f %.2f", chxt[0],chxt[1],chxt[2],chxt[3]);
    SOLGUI_printf(0, 24, F6X8, showbuf);
    sprintf((char*)showbuf, "A:%.2f %.2f %.2f %.2f", chxa[0],chxa[1],chxa[2],chxa[3]);
    SOLGUI_printf(0, 32, F6X8, showbuf);
    sprintf((char*)showbuf, "start-%d,stop-%d", startflag, stopflag);
    SOLGUI_printf(0, 40, F6X8, showbuf);
    sprintf((char*)showbuf, "ch-%d sw-%d", channel, sw);
    SOLGUI_printf(0, 48, F6X8, showbuf);
    SOLGUI_printf(0, 56, F6X8, rxbuf);
    SOLGUI_Refresh();
  }

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(&huart1 == huart)
  {
    // HAL_UART_Transmit_DMA(&huart1, rxbuf, strlen((const char*)rxbuf));
    KDCOM_Process((uint8*)rxbuf, &channel, pchxp, pchxi, pchxd, pchxt, pchxa, &startflag, &stopflag, 0, 0);
    memset(rxbuf, 0, 40);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rxbuf, 40);
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
