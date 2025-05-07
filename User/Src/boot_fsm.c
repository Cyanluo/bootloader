#include <boot.h>
#include <string.h>
#include <usart.h>
#include <usr_sleep.h>
#include <xmodem.h>

static uint8_t ota_condition(void);
static void cmd_info(void);
static void exec_cmd(uint8_t* data, uint16_t len);
static void load_app(uint8_t target_app);
static void jump_to_app();
static void iap_update_a_section(void);
static void set_version(void);
static void get_version(void);
static void download_app_to_w25q(void);
static void erase_a_section(void);
static void receive_file_to_target_addr(uint8_t type, uint32_t addr);
static void load_app_from_w25q(void);

cmd_task_item cmd_tasks[] = {
	{erase_a_section, "擦除A分区"},
	{iap_update_a_section, "串口IAP向A区烧录程序"},
	{NVIC_SystemReset, "重启"},
	{set_version, "设置版本号"},
	{get_version, "查询版本号"},
	{download_app_to_w25q, "向外部Flash下载程序"},
	{load_app_from_w25q, "加载外部Flash程序到A分区"},
};

static uint8_t cmd_task_num = sizeof(cmd_tasks)/sizeof(*cmd_tasks);

void boot_fsm(void)
{
	printf("请在3秒内输入w进入命令行\r\n");
	if (recv_target_msg("w", strlen("w"), 3) != 0)
	{
		if (ota_condition() == 0)
		{
			load_app(0);
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
	for (uint8_t i = 0; i < cmd_task_num; i++)
	{
		printf("%d:%s\r\n", i, cmd_tasks[i].description);
	}
}

void exec_cmd(uint8_t* data, uint16_t len)
{
	if ( len == 1 && cmd_task_num > 0 && data[0] >= '0' && data[0] < ('0' + cmd_task_num) )
	{
		cmd_tasks[data[0] - '0'].task();
	}
	else
	{
		printf("无效输入\r\n");
	}
}

uint8_t ota_condition(void)
{
	return !(boot_flag.OAT_FLAG == OAT_FLAG_VAL);
}

void load_app(uint8_t target_app)
{
	if (load_app_to_a_section(target_app) != 0)
	{
		printf("加载失败\r\n");
	}

	if (target_app == 0)
	{
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

void erase_a_section(void)
{
	printf("开始擦除A分区\r\n");
	if (stm32flash_erase_sector(SECTION_A_START_SECTOR, SECTION_A_SECTOR_NUM) != 0)
	{
		printf("擦除失败\r\n");
		return;
	}
	printf("擦除完毕\r\n");
}

/**
 *
 * @param type 0: to internal flash,
 *			   1: to w25q
 * @param addr if type == 0 : addr is the address
 *			   if type == 1 : addr is the page number
 */
void receive_file_to_target_addr(uint8_t type, uint32_t addr)
{
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

				if (type == 0)
				{
					stm32flash_write(addr+PACKET_PAYLOAD_SIZE*(packet_count-1), \
								 (uint32_t*)packet.payload, PACKET_PAYLOAD_SIZE);
				}
				else if (type == 1)
				{
					memcpy(app_load_info.app_load_buff+(packet_count+1)%2*PACKET_PAYLOAD_SIZE, \
							packet.payload, PACKET_PAYLOAD_SIZE);

					if ( (packet_count+1) % 2 )
					{
						uint32_t a = addr+(packet_count-2)*PACKET_PAYLOAD_SIZE/W25Q_PAGE_SIZE;
						w25q128_write_page(a, \
						                   app_load_info.app_load_buff);
					}
				}

				xmodem_ack();
			}
		}
		else if (recv_state == XMODEM_RECV_END)
		{
			if (type == 1)
			{
				boot_flag.APP_LENS[app_load_info.target_app] = packet_count*PACKET_PAYLOAD_SIZE;
				set_boot_flag(&boot_flag);

				if (packet_count%2)
				{
					w25q128_write_page(addr+(packet_count-1)*PACKET_PAYLOAD_SIZE/W25Q_PAGE_SIZE, \
										   app_load_info.app_load_buff);
				}
			}

			xmodem_ack();
			break;
		}
	}
}

void iap_update_a_section(void)
{
	erase_a_section();

	receive_file_to_target_addr(0, SECTION_A_START_ADDR);

	printf("烧写成功\r\n");
}

void set_version(void)
{
	uint16_t len = 0;
	uint8_t tmp;

	printf("请输入以下格式版本号:");
	HAL_UART_Transmit(&huart1, VERSION_FORMAT, strlen(VERSION_FORMAT), 100);
	printf("\r\n");

	while ( (len = recv_target_len(0, (uint8_t*)fetch_buffer)) == 0)
	{
		msleep(5);
	}

	fetch_buffer[len] = '\0';

	if (sscanf((char*)fetch_buffer, VERSION_FORMAT, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp) == 9)
	{
		memset(boot_flag.VERSION, 0, sizeof(boot_flag.VERSION));
		memcpy(boot_flag.VERSION, fetch_buffer, strlen(fetch_buffer));

		set_boot_flag(&boot_flag);

		printf("版本号设置成功\r\n");
	}
	else
	{
		printf("版本格式错误\r\n");
		set_version();
	}
}

void get_version(void)
{
	get_boot_flag();

	printf("\r\n%s\r\n", (char*)boot_flag.VERSION);
}

void download_app_to_w25q(void)
{
	printf("请输入APP编号:(1~%d)\r\n", MAX_APP_NUM-1);

	while ( recv_target_len(1, (uint8_t*)fetch_buffer) == 0)
	{
		msleep(5);
	}

	if ( fetch_buffer[0] > '0' && fetch_buffer[0] < ('0' + MAX_APP_NUM) )
	{
		app_load_info.target_app = fetch_buffer[0] - '0';

		for (uint32_t i = 0; i < APP_SECTOR_NUM; i++)
		{
			w25q128_sector_erase(i+app_load_info.target_app*APP_SECTOR_NUM);
		}

		receive_file_to_target_addr(1, app_load_info.target_app*APP_PAGE_NUM);

		printf("下载成功\r\n");
	}
	else
	{
		printf("无效输入\r\n");
		download_app_to_w25q();
	}
}

void load_app_from_w25q(void)
{
	printf("请输入APP编号:(1~%d)\r\n", MAX_APP_NUM-1);

	while ( recv_target_len(1, (uint8_t*)fetch_buffer) == 0)
	{
		msleep(5);
	}

	if ( fetch_buffer[0] > '0' && fetch_buffer[0] < ('0' + MAX_APP_NUM) )
	{
		printf("开始加载\r\n");

		load_app(fetch_buffer[0]-'0');

		printf("加载成功\r\n");
	}
	else
	{
		printf("无效输入\r\n");
		load_app_from_w25q();
	}
}
