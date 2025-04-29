#include <boot.h>
#include "retarget.h"
#include <fsm.h>
#include <queue.h>
#include <string.h>
#include <usart.h>
#include <usr_sleep.h>

void boot_init(void)
{
	sector_size_init();
	at24c02_init();
	boot_fsm_register();
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

uint8_t isp_wait(void)
{
	uint16_t time_out = 300;
	uint16_t len = 0;

	while (time_out--)
	{
		DISABLE_INT();
		len = length(data_queue);
		ENABLE_INT();

		if (len >= 4)
		{
			DISABLE_INT();
			fetch(data_queue, len, fetch_buffer);
			ENABLE_INT();

			uint32_t isp_flag;
			memcpy(&isp_flag, fetch_buffer, 4);

			if (isp_flag == 0x44332211)
				return 1;
		}

		msleep(10);
	}

	return 0;
}

uint8_t ota_condition(void)
{
	BootFlags boot_flag = get_boot_flag();

	return boot_flag.OAT_FLAG == OAT_FLAG_VAL;
}

void load_app(uint16_t argc, char* argv[])
{
	if (argc == 0)
	{
		printf("load_isp_app_from_serial_w25q\n");
		printf("...\n");
		printf("load_isp_app_from_w25q_to_a_section\n");
		set_state(ISP_FINISH);
	}
	else if (argc == 1)
	{
		printf("load_ota_app_from_w25q_to_a_section\n");
		set_boot_flag(0);
	}
}

void jump_to_app(uint16_t argc, char* argv[])
{
	printf("jump_to_app\n");
	system_reset();
	load_a_section(SECTION_A_START_ADDR);
}

void boot_fsm_register(void)
{
	FSM_State_Convert state_convert;

	state_convert.pre_state = INIT_STATE;
	state_convert.condition = isp_wait;
	state_convert.state = ISP_DOWNLOADING;
	state_convert.argc = 0;
	state_convert.action = load_app;
	state_convert.r_state = ISP_FINISH;
	state_convert.r_action = nop_action;

	add_fsm_state(&state_convert);

	state_convert.pre_state = ISP_FINISH;
	state_convert.condition = ota_condition;
	state_convert.state = ISP_FINISH;
	state_convert.argc = 1;
	state_convert.action = load_app;
	state_convert.r_state = ISP_FINISH;
	state_convert.r_action = jump_to_app;

	add_fsm_state(&state_convert);
}
