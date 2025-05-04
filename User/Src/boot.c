#include <boot.h>
#include <retarget.h>
#include <fmc.h>

BootFlags boot_flag;
AppLoadInfo app_load_info;

void boot_init(void)
{
	sector_size_init();
	at24c02_init();
	get_boot_flag();
}

BootFlags get_boot_flag(void)
{
	at24c02_read_bytes(0, (uint8_t*)&boot_flag, sizeof(boot_flag));

	return boot_flag;
}

void set_boot_flag(BootFlagsPtr boot_flag)
{
	at24c02_write_bytes(0, (uint8_t*)boot_flag, sizeof(*boot_flag));
}

void MSR_SP(uint32_t sp)
{
	__asm__ volatile (
		"MSR MSP, r0\n\t"
		"BX r14"
	);
}

// addr: SECTION_A_START_ADDR
void jump_to_a_section(uint32_t addr)
{
	if ( *(uint32_t*)addr < RAM_START_ADDR || *(uint32_t*)addr > RAM_END_ADDR )
		return;

	system_reset();

	const pf p_a = (pf)(*(uint32_t*)(addr+4));

	MSR_SP(*(uint32_t*)addr);
	p_a();
}

uint8_t load_app_to_a_section(uint8_t app_num)
{
	if ( app_num > (W25Q_SIZE/MAX_APP_SIZE - 1) )
		return 1;

	if (boot_flag.APP_LENS[app_num] % 4 != 0)
		return 2;

	stm32flash_erase_sector(SECTION_A_START_SECTOR, SECTION_A_SECTOR_NUM);

	uint16_t i = 0;
	for (i=0; i<boot_flag.APP_LENS[app_num]/APP_LOAD_BUFF_SIZE; i++)
	{
		w25q128_read(app_num*MAX_APP_SIZE+i*APP_LOAD_BUFF_SIZE, app_load_info.app_load_buff, APP_LOAD_BUFF_SIZE);
		stm32flash_write(SECTION_A_START_ADDR+i*APP_LOAD_BUFF_SIZE, (uint32_t*)app_load_info.app_load_buff, APP_LOAD_BUFF_SIZE);
	}

	if (boot_flag.APP_LENS[app_num]%APP_LOAD_BUFF_SIZE != 0)
	{
		w25q128_read(app_num*MAX_APP_SIZE+i*APP_LOAD_BUFF_SIZE, app_load_info.app_load_buff, boot_flag.APP_LENS[app_num]%APP_LOAD_BUFF_SIZE);
		stm32flash_write(SECTION_A_START_ADDR+i*APP_LOAD_BUFF_SIZE, (uint32_t*)app_load_info.app_load_buff, boot_flag.APP_LENS[app_num]%APP_LOAD_BUFF_SIZE);
	}

	return 0;
}
