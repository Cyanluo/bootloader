#ifndef BOOT_H
#define BOOT_H

#include <fmc.h>

/*
	Flash 分为 A、B 区
	B区在前，A区在后
	B区放 bootloader
	A区放用户程序
*/

#define SECTION_B_START_SECTOR		0
#define SECTION_B_SECTOR_NUM 		5
#define SECTION_B_START_ADDR		FLASH_BASE
#define SECTION_A_START_SECTOR		SECTION_B_SECTOR_NUM
#define SECTION_A_SECTOR_NUM 		(FLASH_SECTOR_NUM - SECTION_B_SECTOR_NUM)
#define SECTION_A_START_ADDR 		get_sector_addr(SECTION_A_START_SECTOR)

#define	 OAT_FLAG_VAL	0x12345678

typedef struct
{
	uint32_t OAT_FLAG;
} BootFlags, *BootFlagsPtr;

void boot_init(void);
BootFlags get_boot_flag(void);
void set_boot_flag(BootFlagsPtr boot_flag);

#endif //BOOT_H
