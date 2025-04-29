#ifndef BOOT_H
#define BOOT_H

#include <fmc.h>
#include <at24c02.h>

/*
	将 Flash 分为 A、B 区
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

#define RAM_START_ADDR				0x20000000
#define RAM_END_ADDR 				0x20020000

#define	 OAT_FLAG_VAL	0x11223344

typedef void(*pf)(void);

typedef struct
{
	uint32_t OAT_FLAG;
	uint32_t APP_LENS[5];
	uint8_t	 TARGET_APP;
} BootFlags, *BootFlagsPtr;

void boot_init(void);
void boot_fsm(void);
BootFlags get_boot_flag(void);
void set_boot_flag(BootFlagsPtr boot_flag);
void system_reset(void);
void load_a_section(uint32_t addr);

static void load_app(uint16_t argc, char* argv[]);
static void boot_fsm_register(void);
static void jump_to_app(uint16_t argc, char* argv[]);

#endif //BOOT_H
