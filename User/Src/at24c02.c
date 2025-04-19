#include <at24c02.h>
#include <i2c.h>
#include <usr_sleep.h>

void at24c02_init(void)
{
	i2c_init();
}

uint8_t at24c02_write_byte(uint8_t addr, uint8_t data)
{
	i2c_start();

	i2c_write_byte(AT24C02_WADDR);
	if (i2c_wait_ack(100) != 0)
		return 1;

	i2c_write_byte(addr);
	if (i2c_wait_ack(100) != 0)
		return 2;

	i2c_write_byte(data);
	if (i2c_wait_ack(100) != 0)
		return 3;

	i2c_stop();

	msleep(5);

	return 0;
}

uint8_t at24c02_write_bytes(uint8_t addr, uint8_t* data, uint16_t len)
{
	int ret = 0;

	for (uint16_t i = 0; i < len && ret == 0; i++)
	{
		ret = at24c02_write_byte(addr+i, data[i]);
	}

	return ret;
}

uint8_t at24c02_write_page(uint8_t addr, uint8_t *data)
{
	i2c_start();

	i2c_write_byte(AT24C02_WADDR);
	if (i2c_wait_ack(100) != 0)
		return 1;

	i2c_write_byte(addr);
	if (i2c_wait_ack(100) != 0)
		return 2;

	for (int8_t i = 0; i < 8; i++)
	{
		i2c_write_byte(data[i]);
		if (i2c_wait_ack(100) != 0)
			return 3 + i;
	}

	i2c_stop();

	msleep(10);

	return 0;
}

uint8_t at24c02_read_bytes(uint8_t addr, uint8_t *data, uint16_t len)
{
	i2c_start();

	i2c_write_byte(AT24C02_WADDR);
	if (i2c_wait_ack(100) != 0)
		return 1;

	i2c_write_byte(addr);
	if (i2c_wait_ack(100) != 0)
		return 2;

	i2c_start();

	i2c_write_byte(AT24C02_RADDR);
	if (i2c_wait_ack(100) != 0)
		return 3;

	for (uint16_t i = 0; i < len-1; i++)
	{
		data[i] = i2c_read_byte(1);
	}

	data[len-1] = i2c_read_byte(0);

	i2c_stop();

	return 0;
}
