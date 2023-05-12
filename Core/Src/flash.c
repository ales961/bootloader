#include "flash.h"



void FlashWriteByte(uint32_t Address, uint8_t Data) {
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, Address, Data);
	HAL_FLASH_Lock();
}

void FlashWriteWord(uint32_t Address, uint32_t Data) {
	HAL_StatusTypeDef status = HAL_ERROR;
	HAL_FLASH_Unlock();
	while (status != HAL_OK)
		status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, Data);
	HAL_FLASH_Lock();
}

uint8_t FlashReadByte(uint32_t Address) {
    return (*(__IO uint8_t*)Address);
}

uint32_t FlashReadWord(uint32_t Address) {
    return (*(__IO uint32_t*)Address);
}

void EraseSector(uint32_t Sector) {
	HAL_StatusTypeDef status = HAL_ERROR;
	HAL_FLASH_Unlock();
	uint32_t SectorError;
	static FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.Sector = Sector;
	EraseInitStruct.NbSectors = 1 ;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	while (status != HAL_OK)
		status = HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	HAL_FLASH_Lock();
}
