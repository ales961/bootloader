#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"
#include "flash.h"

uint32_t ptr = 0;

void FlashWriteBytes(uint32_t Address, uint8_t* Data, uint16_t size) {
	HAL_FLASH_Unlock();
	for (uint16_t i = 0; i < size; i++) {
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Address + i, Data[i]);
	}
	HAL_FLASH_Lock();
}

void FlashWriteWord(uint32_t Address, uint32_t Data) {
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Data);
	HAL_FLASH_Lock();
}

uint8_t FlashReadByte(uint32_t Address)
{
    return (*(__IO uint8_t*)Address);
}

void EraseSector(uint32_t Sector) {
	HAL_FLASH_Unlock();
	uint32_t SectorError;
	static FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector = Sector;
	EraseInitStruct.NbSectors = 1 ;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	HAL_FLASH_Lock();
}
