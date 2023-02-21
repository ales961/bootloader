#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#define SECTOR_0_ADDRESS (uint32_t)0x08000000
#define SECTOR_1_ADDRESS (uint32_t)0x08004000
#define SECTOR_2_ADDRESS (uint32_t)0x08008000
#define SECTOR_3_ADDRESS (uint32_t)0x0800C000
#define SECTOR_4_ADDRESS (uint32_t)0x08010000
#define SECTOR_5_ADDRESS (uint32_t)0x08020000
#define SECTOR_6_ADDRESS (uint32_t)0x08040000
#define SECTOR_7_ADDRESS (uint32_t)0x08060000
#define SECTOR_8_ADDRESS (uint32_t)0x08080000
#define SECTOR_9_ADDRESS (uint32_t)0x080A0000
#define SECTOR_10_ADDRESS (uint32_t)0x080C0000
#define SECTOR_11_ADDRESS (uint32_t)0x080E0000
#define SECTOR_12_ADDRESS (uint32_t)0x08100000
#define SECTOR_13_ADDRESS (uint32_t)0x08104000
#define SECTOR_14_ADDRESS (uint32_t)0x08108000
#define SECTOR_15_ADDRESS (uint32_t)0x0810C000
#define SECTOR_16_ADDRESS (uint32_t)0x08110000
#define SECTOR_17_ADDRESS (uint32_t)0x08120000
#define SECTOR_18_ADDRESS (uint32_t)0x08140000
#define SECTOR_19_ADDRESS (uint32_t)0x08160000
#define SECTOR_20_ADDRESS (uint32_t)0x08180000
#define SECTOR_21_ADDRESS (uint32_t)0x081A0000
#define SECTOR_22_ADDRESS (uint32_t)0x081C0000
#define SECTOR_23_ADDRESS (uint32_t)0x081E0000

void FlashWriteByte(uint32_t Address, uint8_t Data);
void FlashWriteWord(uint32_t Address, uint32_t Data);
uint8_t FlashReadByte(uint32_t Address);
uint32_t FlashReadWord(uint32_t Address);
void EraseSector(uint32_t Sector);

#endif /* INC_FLASH_H_ */
