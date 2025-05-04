#ifndef XMODEM_H
#define XMODEM_H

#include <stm32f4xx_hal.h>

/*
	|  byte 1  |   	  byte 2	 |       byte 3  	|   byte 4~131  |  byte 132   |   byte 133  |
	  |   SOH    |  packet number  | ~(packet number) |  packet data  |    CRCH     |     CRCL    |
*/

#define PACKET_PAYLOAD_SIZE 128

#define SOH 	0x01 // modem数据头
#define	ACK 	0x06 // 应答
#define	NAK 	0x15 // 非应答
#define	EOT 	0x04 // 发送结束
#define	CAN 	0x18 // 取消发送
#define FILL    0x1A // 最后不足128字节的部分填充值

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	uint8_t soh;
	uint8_t packet_number;
	uint8_t reverse_packet_number;
	uint8_t payload[PACKET_PAYLOAD_SIZE];
	uint8_t crch;
	uint8_t crcl;
} XmodemPacket, *XmodemPacketPtr;
#pragma pack(pop)

enum XMODEM_RECV_STATE
{
	XMODEM_RECV_SOH_ERROR,
	XMODEM_RECV_CRC_ERROR,
	XMODEM_RECV_SUCCESS,
	XMODEM_RECV_END
};

uint16_t crc16_xmodem(uint8_t *data, uint16_t length);
void request_file(void);
enum XMODEM_RECV_STATE xmodem_disassemble(XmodemPacketPtr xmodem_packet);
void xmodem_nak();
void xmodem_ack();

#endif //XMODEM_H
