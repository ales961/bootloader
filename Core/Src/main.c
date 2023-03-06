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
#include "lwip.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "flash.h"
#include "hex_parser.h"
#include "xmodem.h"
#include "command.h"
#include "menu.h"
#include "boot_config.h"
#include "tcp.h"
#include "ethernetif.h"
#include "lwip_callbacks.h"
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
static uint8_t uartBuf[128];
static uint8_t uartBufLast = 0;
static uint8_t hasLine = 0;
static Command* commands[5];
ip4_addr_t ip_addr;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void receiveAndSendChar();
static void sendCommandResult(const char* commandResult);
static void sendMessage(const char* msg);
static char* jumpToUserApp();
static char* getHelpInfo();
static char* downloadFirmware(uint32_t* version);
static char* getAppVersions();
static char* eraseConfigs();
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
  commands[0] = commandCreate("jump", (CommandAction) jumpToUserApp, NONE);
  commands[1] = commandCreate("update", (CommandAction) downloadFirmware, INT);
  commands[2] = commandCreate("version", (CommandAction) getAppVersions, NONE);
  commands[3] = commandCreate("help", (CommandAction) getHelpInfo, NONE);
  commands[4] = commandCreate("clear", (CommandAction) eraseConfigs, NONE);
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
  MX_LWIP_Init();
  /* USER CODE BEGIN 2 */
  uartEnableInterruption();
  menuInit(commands, 5);


  struct tcp_pcb* tcp_pcb = tcp_new();
  if(tcp_pcb != NULL) {
	  err_t err;
	  IP4_ADDR(&ip_addr, 192, 168, 1, 193);
	  err = tcp_bind(tcp_pcb, &ip_addr, 80);
	  if (err == ERR_OK) {
		  tcp_pcb = tcp_listen(tcp_pcb);
		  tcp_accept(tcp_pcb, tcp_accept_callback);
	  } else {
		  memp_free(MEMP_TCP_PCB, tcp_pcb);
	  }

  }
  //tcp_arg(tcp_pcb, ?);

  //tcp_sent(tcp_pcb, ?);
  //tcp_recv(tcp_pcb, ?);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
	  HAL_IWDG_Refresh(&hiwdg);
	  MX_LWIP_Process(); // ethernetif_input(&gnetif) + sys_check_timeouts()

	  if (uartHasNext()) {
		  receiveAndSendChar();
	  }

	  if (hasLine) {
		  uartBuf[uartBufLast] = '\0';
		  const char* commandResult = menuExecuteCommand((char*) uartBuf);
		  sendMessage("\n");
		  sendCommandResult(commandResult);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
static char* jumpToUserApp() {
	validateApplications();
	if (getLatestApplicationAddress() == 0) {
		return "No valid application in flash\n";
	} else {
		jumpToApp();
		return "Done";
	}
}

static char* getHelpInfo() {
    return "jump: jump to application\n\
update <version>: download firmware and jump to it\n\
version: get current version of application\n\
help: get information about commands\n\
clear: erase configs\n";
}

static char* downloadFirmware(uint32_t* version) {
	validateApplications();
	updateConfig();
	setAppVersion(*version);
	sendMessage("Send hex file via XMODEM\n");
	HAL_Delay(100);
	uint8_t xmodemStatus = xmodemReceive();
	if (xmodemStatus == 1) {
		setCorrectUpdateFlag();
		return jumpToUserApp();
	} else if (xmodemStatus == 2) {
		NVIC_SystemReset(); //TODO
		return "Error. Choose firmware for another bank.\n";
	} else {
		NVIC_SystemReset(); //TODO
		return "Error. Update aborted.\n";
	};
}

static char* getAppVersions() {
	validateApplications();
	return getVersions();
}

static char* eraseConfigs() {
	EraseSector(CONFIG_1_SECTOR);
	EraseSector(CONFIG_2_SECTOR);
	return "Configs erased\n";
}

static void receiveAndSendChar() {
    if (uartReceive(uartBuf + uartBufLast, 1)) {
        uint8_t received = uartBuf[uartBufLast];
        uartBufLast++;
        uartTransmit(&received, 1);

        if (received == '\r')
            hasLine = 1;
    }
}

static void sendCommandResult(const char* commandResult) {
    uartTransmit((uint8_t *) commandResult, strlen(commandResult));
    uartBufLast = 0;
    hasLine = 0;
}

static void sendMessage(const char* msg) {
    uartTransmit((uint8_t *) msg, strlen(msg));
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
