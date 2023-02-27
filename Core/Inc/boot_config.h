#ifndef INC_BOOT_CONFIG_H_
#define INC_BOOT_CONFIG_H_

#include "flash.h"

#define CONFIG_1_SECTOR FLASH_SECTOR_2
#define CONFIG_2_SECTOR FLASH_SECTOR_12
#define CONFIG_1_ADDRESS SECTOR_2_ADDRESS
#define CONFIG_2_ADDRESS SECTOR_12_ADDRESS
#define APP_1_ADDRESS SECTOR_3_ADDRESS
#define APP_2_ADDRESS SECTOR_13_ADDRESS

#define EMPTY (uint32_t) 0xFFFFFFFF

char* getVersions();
void jumpToApp();
uint32_t getLatestApplicationAddress();
void updateConfig();
void validateApplications();
void eraseLogicalBank();
void rollbackConfig();

#endif /* INC_BOOT_CONFIG_H_ */