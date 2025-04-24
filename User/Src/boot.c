#include <boot.h>
#include <at24c02.h>

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
