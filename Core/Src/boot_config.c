#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "stm32f4xx_hal.h"

#include "boot_config.h"
#include "flash.h"

uint32_t seqAddress1 = CONFIG_1_ADDRESS;
uint32_t firstBootFlagAddress1 = CONFIG_1_ADDRESS + 4;
uint32_t notValidFlagAddress1 = CONFIG_1_ADDRESS + 8;
uint32_t versionAddress1 = CONFIG_1_ADDRESS + 12;
uint32_t notCorrectUpdateFlagAddress1 = CONFIG_1_ADDRESS + 16;

uint32_t seqAddress2 = CONFIG_2_ADDRESS;
uint32_t firstBootFlagAddress2 = CONFIG_2_ADDRESS + 4;
uint32_t notValidFlagAddress2 = CONFIG_2_ADDRESS + 8;
uint32_t versionAddress2 = CONFIG_2_ADDRESS + 12;
uint32_t notCorrectUpdateFlagAddress2 = CONFIG_2_ADDRESS + 16;

static char versionBuf[128];
char* getVersions() {
	uint32_t ver1 = FlashReadWord(versionAddress1);
	uint32_t ver2 = FlashReadWord(versionAddress2);
	if (ver1 == EMPTY && ver2 == EMPTY) {
		return "No applications in flash\n";
	} else if (ver1 == EMPTY) {
		sprintf(versionBuf, "App 1 version: none\nApp 2 version: %"PRIu32"\n", ver2);
		return versionBuf;
	} else if (ver2 == EMPTY) {
		sprintf(versionBuf, "App 1 version: %"PRIu32"\nApp 2 version: none\n", ver1);
		return versionBuf;
	} else {
		sprintf(versionBuf, "App 1 version: %"PRIu32"\nApp 2 version: %"PRIu32"\n", ver1, ver2);
		return versionBuf;
	}
}

uint32_t getLatestApplicationAddress() {
	if (FlashReadWord(seqAddress1) < FlashReadWord(seqAddress2)) {
		return APP_1_ADDRESS;
	} else if (FlashReadWord(seqAddress2) < FlashReadWord(seqAddress1)) {
		return APP_2_ADDRESS;
	} else {
		return 0;
	}
}

void updateConfig() {
	uint32_t latestAppAddress = getLatestApplicationAddress();
	if (latestAppAddress == APP_1_ADDRESS) {
		EraseSector(CONFIG_2_SECTOR);
		FlashWriteWord(seqAddress2, FlashReadWord(seqAddress1) - 1);
		//FlashWriteWord(firstBootFlagAddress2, 0xFFFF); // FFFF - first boot (default)
		//FlashWriteWord(notValidFlagAddress2, 0xFFFF); // FFFF - not valid (default)
	} else {
		EraseSector(CONFIG_1_SECTOR);
		FlashWriteWord(seqAddress1, FlashReadWord(seqAddress2) - 1);
		//FlashWriteWord(firstBootFlagAddress1, 0xFFFF); // FFFF - first boot (default)
		//FlashWriteWord(notValidFlagAddress1, 0xFFFF); // FFFF - not valid (default)
	}
}

void setAppVersion(uint32_t version) {
	uint32_t latestAppAddress = getLatestApplicationAddress();
		if (latestAppAddress == APP_1_ADDRESS) {
			FlashWriteWord(versionAddress1, version);
		} else {
			FlashWriteWord(versionAddress2, version);
		}
}

void setCorrectUpdateFlag() {
	uint32_t latestAppAddress = getLatestApplicationAddress();
		if (latestAppAddress == APP_1_ADDRESS) {
			FlashWriteWord(notCorrectUpdateFlagAddress1, 0);
		} else {
			FlashWriteWord(notCorrectUpdateFlagAddress2, 0);
		}
}

void validateApplications() {
	uint32_t nValid1 = FlashReadWord(notValidFlagAddress1);
	uint32_t firstBoot1 = FlashReadWord(firstBootFlagAddress1);
	uint32_t nCorrect1 = FlashReadWord(notCorrectUpdateFlagAddress1);
	uint32_t nValid2 = FlashReadWord(notValidFlagAddress2);
	uint32_t firstBoot2 = FlashReadWord(firstBootFlagAddress2);
	uint32_t nCorrect2 = FlashReadWord(notCorrectUpdateFlagAddress2);
	if ((nValid1 == EMPTY && firstBoot1 == 0) || nCorrect1 != 0)
		EraseSector(CONFIG_1_SECTOR);
	if ((nValid2 == EMPTY && firstBoot2 == 0) || nCorrect2 != 0)
		EraseSector(CONFIG_2_SECTOR);
}


void EraseNecessarySectors(uint32_t address, uint8_t* sector) {
	if (address == sectorAddresses[*sector]) {
		EraseSector(*sector);
		(*sector)++;
	}
}

void jumpToApp() {
	  uint32_t address = getLatestApplicationAddress();
	  if (address == APP_1_ADDRESS)
		  FlashWriteWord(firstBootFlagAddress1, 0);
	  else if (address == APP_2_ADDRESS)
		  FlashWriteWord(firstBootFlagAddress2, 0);

	  void(*app_reset_handler)();

	  //shut down any tasks remaining
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

	  app_reset_handler = (void (*)(void)) (resethandler_address);

	  //jump to reset handler of the user app.
	  __enable_irq();
	  app_reset_handler();
}
