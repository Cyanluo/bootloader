#include <boot.h>
#include <queue.h>
#include <string.h>
#include <usart.h>
#include <usr_sleep.h>

static uint8_t ota_condition(void);
static void cmd_info(void);
static void exec_cmd(uint8_t* data, uint16_t len);
static uint8_t recv_target_msg(const uint8_t* target, uint8_t size, uint16_t timeout);
static void load_app(int type);
static void jump_to_app();

// timeout: xx second
uint8_t recv_target_msg(const uint8_t* target, uint8_t size, uint16_t timeout)
{
	timeout *= 100;
	uint16_t len = 0;

	while (timeout--)
	{
		DISABLE_INT();
		len = length(data_queue);
		ENABLE_INT();

		if (len >= size)
		{
			DISABLE_INT();
			fetch(data_queue, size, fetch_buffer);
			ENABLE_INT();

			uint8_t i = 0;
			for (i = 0; i < size; i++)
			{
				if (fetch_buffer[i] != target[i])
					break;
			}

			return i == size ? 0 : 1;
		}

		msleep(10);
	}

	return 2;
}

uint8_t ota_condition(void)
{
	return !(boot_flag.OAT_FLAG == OAT_FLAG_VAL);
}

void load_app(int type)
{
	if (type == 0)
	{
		printf("load_iap_app_from_serial_w25q\r\n");
		printf("...\r\n");
		printf("load_iap_app_from_w25q_to_a_section\r\n");
	}
	else if (type == 1)
	{
		printf("load_ota_app_from_w25q_to_a_section\r\n");
		boot_flag.OAT_FLAG = 0;
		set_boot_flag(&boot_flag);
	}
}

void jump_to_app()
{
	printf("跳转A分区\r\n");

	jump_to_a_section(SECTION_A_START_ADDR);

	printf("跳转A分区失败\r\n");
}

void cmd_info(void)
{
	printf("\r\n");
	printf("1.擦除A分区\r\n");
	printf("2.串口IAP下载A区程序\r\n");
	printf("3.重启\r\n");
}

void cmd_loop()
{
	DISABLE_INT();
	uint16_t len = length(data_queue);
	ENABLE_INT();

	if (len > 0)
	{
		DISABLE_INT();
		fetch(data_queue, len, fetch_buffer);
		ENABLE_INT();

		exec_cmd(fetch_buffer, len);

		cmd_info();
	}
}

void exec_cmd(uint8_t* data, uint16_t len)
{
	if (len == 1 && data[0] >= '1' && data[0] <= '3')
	{
		if (data[0] == '1')
		{
			printf("开始擦除\r\n");
			if (stm32flash_erase_sector(SECTION_A_START_SECTOR, SECTION_A_SECTOR_NUM) != 0)
				printf("擦除失败\r\n");
			else
				printf("擦除完毕\r\n");
		}
		else if (data[0] == '3')
		{
			NVIC_SystemReset();
		}
	}
}

void boot_fsm(void)
{
	printf("请在3秒内输入w进入命令行\r\n");
	if (recv_target_msg("w", strlen("w"), 3) != 0)
	{
		if (ota_condition() == 0)
		{
			load_app(1);
			NVIC_SystemReset();
		}
		else
		{
			jump_to_app();
		}
	}

	printf("进入命令行\r\n");
	cmd_info();
}
