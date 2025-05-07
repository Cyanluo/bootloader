#ifndef BOOT_H
#define BOOT_H

#include <at24c02.h>
#include <fmc.h>
#include <w25q128.h>

/**
  * Area B is in front, area A is in the back
  * Area B is for bootloader
  * Area A is for user program
*/

#define SECTION_B_START_SECTOR		0
#define SECTION_B_SECTOR_NUM 		5
#define SECTION_B_START_ADDR		FLASH_BASE
#define SECTION_A_START_SECTOR		SECTION_B_SECTOR_NUM
#define SECTION_A_SECTOR_NUM 		(FLASH_SECTOR_NUM - SECTION_B_SECTOR_NUM)
#define SECTION_A_START_ADDR 		get_sector_addr(SECTION_A_START_SECTOR)

#define RAM_START_ADDR				0x20000000
#define RAM_END_ADDR 				0x20020000

#define	 OAT_FLAG_VAL		0x11223344

#define MAX_APP_SIZE		(64*1024)		// Byte
#define APP_LOAD_BUFF_SIZE	512				// Byte
#define MAX_APP_NUM			9
#define APP_SECTOR_NUM		(MAX_APP_SIZE/W25Q_SECTOR_SIZE)
#define APP_PAGE_NUM		(MAX_APP_SIZE/W25Q_PAGE_SIZE)

#define VERSION_FORMAT	"VER-v%d.%d.%d-%d.%d.%d-%d.%d.%d"	 // "VER-v0.0.1-2025.5.01-12.12.12"

typedef void(*pf)(void);

typedef struct
{
	uint32_t OAT_FLAG;
	uint32_t APP_LENS[MAX_APP_NUM];  // max 64k per app
	uint8_t  VERSION[40];
} BootFlags, *BootFlagsPtr;

typedef struct
{
	uint16_t target_app;
	uint8_t  app_load_buff[APP_LOAD_BUFF_SIZE];
} AppLoadInfo;

extern BootFlags	boot_flag;
extern AppLoadInfo	app_load_info;

typedef struct
{
	void (*task)(void);
	const char *description;
} cmd_task_item;

void boot_init(void);
void boot_fsm(void);
void cmd_loop();
BootFlags get_boot_flag(void);
void set_boot_flag(BootFlagsPtr boot_flag);
void system_reset(void);
void jump_to_a_section(uint32_t addr);
uint8_t load_app_to_a_section(uint8_t app_num);

#endif //BOOT_H
