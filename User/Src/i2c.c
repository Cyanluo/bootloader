#include <i2c.h>
#include <usr_sleep.h>

void i2c_init(void)
{
	I2C_CLOCK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = I2C_SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = I2C_SDA_Pin;
	HAL_GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStruct);

	I2C_SCL_H();
	I2C_SDA_H();
}

void i2c_start(void)
{
	I2C_SCL_H();
	I2C_SDA_H();
	usleep(2);
	I2C_SDA_L();
	usleep(2);
	I2C_SCL_L();
}

void i2c_stop(void)
{
	I2C_SCL_L();
	I2C_SDA_L();
	usleep(2);
	I2C_SCL_H();
	usleep(1);
	I2C_SDA_H();
	usleep(2);
}

void i2c_write_byte(uint8_t data)
{
	for (int8_t i = 7; i >= 0; i--)
	{
		I2C_SCL_L();

		if (data & (1 << i))
			I2C_SDA_H();
		else
			I2C_SDA_L();

		usleep(2);
		I2C_SCL_H();
		usleep(2);
	}

	I2C_SCL_L();
	I2C_SDA_H();
}

uint8_t i2c_wait_ack(int16_t timeout)
{
	uint8_t ret = 0;

	I2C_SDA_H();
	usleep(1);
	I2C_SCL_H();
	usleep(1);

	do
	{
		timeout--;
		usleep(2);
	} while ((READ_I2C_SDA() == 1) && (timeout >= 0));

	if (timeout < 0)
	{
		ret = 1;
	}
	else
	{
		I2C_SCL_H();
		usleep(2);

		if (READ_I2C_SDA() != 0)
			ret = 2;
		else
		{
			I2C_SCL_L();
			usleep(2);
		}
	}

	return ret;
}

uint8_t i2c_read_byte(uint8_t ack)
{
	uint8_t ret = 0;

	for (int8_t i = 7; i >= 0; i--)
	{
		I2C_SCL_L();
		usleep(2);
		I2C_SCL_H();
		if (READ_I2C_SDA() == 1)
			ret |= (1 << i);
		usleep(2);
	}

	I2C_SCL_L();
	usleep(2);

	if (ack)
	{
		I2C_SDA_L();
	}
	else
	{
		I2C_SDA_H();
	}

	I2C_SCL_H();
	usleep(2);
	I2C_SCL_L();
	usleep(2);
	I2C_SDA_H();

	return ret;
}
