#include <boot.h>
#include <string.h>
#include <usart.h>
#include <usr_sleep.h>
#include <xmodem.h>

static uint8_t ota_condition(void);
static void cmd_info(void);
static void exec_cmd(uint8_t* data, uint16_t len);
static void load_app(int type);
static void jump_to_app();
static void iap_update_a_section(void);

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

void cmd_loop()
{
	uint16_t len;
	if ( (len = recv_target_len(0, fetch_buffer)) > 0 )
	{
		exec_cmd(fetch_buffer, len);
		cmd_info();
	}
}

void cmd_info(void)
{
	printf("\r\n");
	printf("1.擦除A分区\r\n");
	printf("2.串口IAP向A区烧录程序\r\n");
	printf("3.重启\r\n");
}

void exec_cmd(uint8_t* data, uint16_t len)
{
	if (len == 1 && data[0] >= '1' && data[0] <= '3')
	{
		if (data[0] == '1')
		{
			printf("开始擦除A分区\r\n");
			if (stm32flash_erase_sector(SECTION_A_START_SECTOR, SECTION_A_SECTOR_NUM) != 0)
				printf("擦除失败\r\n");
			else
				printf("擦除完毕\r\n");
		}
		else if (data[0] == '2')
		{
			iap_update_a_section();
			printf("烧写成功\r\n");
		}
		else if (data[0] == '3')
		{
			NVIC_SystemReset();
		}
	}
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

void iap_update_a_section(void)
{
	printf("开始擦除A分区\r\n");
	if (stm32flash_erase_sector(SECTION_A_START_SECTOR, SECTION_A_SECTOR_NUM) != 0)
	{
		printf("擦除失败\r\n");
		return;
	}
	printf("擦除完毕\r\n");

	printf("请使用 xmodem 协议发送 bin 文件\r\n");

	request_file();

	uint8_t last_packet_number = 0;
	uint16_t packet_count = 0;
	XmodemPacket packet;
	enum XMODEM_RECV_STATE recv_state;

	while (1)
	{
		recv_state = xmodem_disassemble(&packet);
		if ( recv_state == XMODEM_RECV_SUCCESS )
		{
			if ( (last_packet_number>0) && (last_packet_number==packet.packet_number) )
			{
				xmodem_nak();
			}
			else
			{
				packet_count++;
				last_packet_number = packet.packet_number;

				stm32flash_write(SECTION_A_START_ADDR+PACKET_PAYLOAD_SIZE*(packet_count-1), \
								 (uint32_t*)packet.payload, PACKET_PAYLOAD_SIZE);

				xmodem_ack();
			}
		}
		else if (recv_state == XMODEM_RECV_END)
		{
			xmodem_ack();
			break;
		}
	}
}
