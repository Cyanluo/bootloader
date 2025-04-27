#include <boot.h>
#include "retarget.h"

void boot_init(void)
{
	sector_size_init();
	at24c02_init();
}

BootFlags get_boot_flag(void)
{
	BootFlags boot_flag = {0};

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
void load_a_section(uint32_t addr)
{
	if ( *(uint32_t*)addr < RAM_START_ADDR || *(uint32_t*)addr > RAM_END_ADDR )
		return;

	const pf p_a = (pf)(*(uint32_t*)(addr+4));

	MSR_SP(*(uint32_t*)addr);
	p_a();
}
