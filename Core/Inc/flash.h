#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#define SECTOR_16_ADDRESS (uint32_t)0x08110000
#define SECTOR_16_ADDRESS_SHORT (uint32_t)0x8110000
#define SECTOR_17_ADDRESS (uint32_t)0x08120000
#define SECTOR_17_ADDRESS_SHORT (uint32_t)0x8120000

void FlashWriteByte(uint32_t Address, uint8_t Data);
void FlashWriteWord(uint32_t Address, uint32_t Data);
uint8_t FlashReadByte(uint32_t Address);
void EraseSector(uint32_t Sector);

#endif /* INC_FLASH_H_ */
