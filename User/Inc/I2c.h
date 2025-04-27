#ifndef I2C_H
#define I2C_H

#include "stm32f4xx_hal.h"

#define I2C_CLOCK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE();
#define I2C_CLOCK_DISABLE()		__HAL_RCC_GPIOB_CLK_DISABLE();
#define I2C_SCL_GPIO_PORT		GPIOB
#define I2C_SCL_Pin				GPIO_PIN_8
#define I2C_SDA_GPIO_PORT		GPIOB
#define I2C_SDA_Pin				GPIO_PIN_9

#define I2C_SCL_H() HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_Pin, GPIO_PIN_SET)
#define I2C_SCL_L() HAL_GPIO_WritePin(I2C_SCL_GPIO_PORT, I2C_SCL_Pin, GPIO_PIN_RESET)
#define I2C_SDA_H() HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_Pin, GPIO_PIN_SET)
#define I2C_SDA_L() HAL_GPIO_WritePin(I2C_SDA_GPIO_PORT, I2C_SDA_Pin, GPIO_PIN_RESET)
#define READ_I2C_SCL() HAL_GPIO_ReadPin(I2C_SCL_GPIO_PORT, I2C_SCL_Pin)
#define READ_I2C_SDA() HAL_GPIO_ReadPin(I2C_SDA_GPIO_PORT, I2C_SDA_Pin)

void i2c_init(void);
void i2c_de_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write_byte(uint8_t data);
uint8_t i2c_wait_ack(int16_t timeout);
uint8_t i2c_read_byte(uint8_t ack);

#endif //I2C_H
