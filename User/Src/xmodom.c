#include <queue.h>
#include <xmodem.h>
#include <retarget.h>
#include <usart.h>
#include <usr_sleep.h>

uint16_t crc16_xmodem(uint8_t *data, uint16_t length)
{
	uint8_t i;
	uint16_t crc = 0;            // Initial value
	while(length--)
	{
		crc ^= (uint16_t)(*data++) << 8; // crc ^= (uint16_t)(*data)<<8; data++;
		for (i = 0; i < 8; ++i)
		{
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc <<= 1;
		}
	}
	return crc;
}

void request_file(void)
{
	while (1)
	{
		printf("C");
		msleep(1000);

		if (length(data_queue) > 0)
			break;
	}
}

enum XMODEM_RECV_STATE xmodem_disassemble(XmodemPacketPtr xmodem_packet)
{
	uint8_t ret = XMODEM_RECV_SOH_ERROR;
	uint16_t len = 0;

	while ( (len = recv_target_len(0, (uint8_t*)xmodem_packet)) == 0)
	{
		msleep(5);
	}

	if (len == 133 && xmodem_packet->soh == SOH)
	{
		const uint16_t crc = crc16_xmodem(xmodem_packet->payload, 128);

		if ( crc == (xmodem_packet->crch << 8 | xmodem_packet->crcl) )
		{
			ret = XMODEM_RECV_SUCCESS;
		}
		else
		{
			xmodem_nak();
			ret = XMODEM_RECV_CRC_ERROR;
		}
	}
	else if (len == 1 && xmodem_packet->soh == EOT)
	{
		ret = XMODEM_RECV_END;
	}

	return ret;
}

void xmodem_ack()
{
	printf("\x06");
}

void xmodem_nak()
{
	printf("\x15");
}
