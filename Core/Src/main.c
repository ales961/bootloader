/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "uart.h"
#include "flash.h"
#include "hex_parser.h"
#include "button.h"
#include "xmodem.h"
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
uint32_t flashBufPtr = 0;
uint8_t loadApp = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void jumpToUserApp(uint32_t address);
static void onButtonClick();
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
  uartInit();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART6_UART_Init();
  MX_TIM6_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  uartEnableInterruption();
  buttonInit(GPIOC, GPIO_PIN_15);
  buttonSetOnClick(&onButtonClick);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */


  uartTransmit("started\n\r", 9);
  HAL_TIM_Base_Start_IT(&htim6); //send "C" every 10 sec
  while (!isRxStarted()) {
	  HAL_IWDG_Refresh(&hiwdg);
	  buttonUpdateState();
	  if (!loadApp) goto jumpToApp;
  }
  EraseSector(FLASH_SECTOR_17);
  HAL_TIM_Base_Stop_IT(&htim6);
  xmodemReceive();

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  jumpToApp:
  jumpToUserApp(SECTOR_17_ADDRESS);
  while (1) {}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void jumpToUserApp(uint32_t address) {
	  void(*app_reset_handler)();

	  //shut down any tasks remaining
	  HAL_TIM_Base_Stop_IT(&htim6);
	  uartDisableInterruption();

	  __HAL_RCC_GPIOC_CLK_DISABLE();
	  __HAL_RCC_GPIOD_CLK_DISABLE();
	  __HAL_RCC_GPIOB_CLK_DISABLE();
	  __HAL_RCC_GPIOA_CLK_DISABLE();

	  HAL_RCC_DeInit();// to turn off the PLL and set the clock to it's default state
	  HAL_DeInit();// to disable all the peripherals

	  SysTick->CTRL = 0;//to turn off the systick
	  SysTick->LOAD = 0;
	  SysTick->VAL = 0;

	  //disable interrupts
	  __set_PRIMASK(1);
	  __disable_irq();

	  //__DMB(); // Data Memory Barrier to ensure write to memory is completed
	  SCB->VTOR = address;//change this
      //__DSB(); // Data Synchronization Barrier to ensure all subsequence instructions use the new configuation

	  //configure the MSP by reading the value from the base address
	  uint32_t msp_value = *(__IO uint32_t*) address;

	  __set_MSP(msp_value);

	  uint32_t resethandler_address = *(__IO uint32_t*) (address + 4);

	  //app_reset_handler = (void*)resethandler_address;
	  app_reset_handler = (void (*)(void)) (resethandler_address);

	  //jump to reset handler of the user app.
	  //NVIC_SystemReset(); //?????
	  __enable_irq();
	  app_reset_handler();
}

static void onButtonClick() {
    loadApp = 0;
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
