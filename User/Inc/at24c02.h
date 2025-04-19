#ifndef AT24C02_H
#define AT24C02_H

#include "stm32f4xx_hal.h"

#define AT24C02_RADDR 0xA1
#define AT24C02_WADDR 0xA0

void at24c02_init(void);
uint8_t at24c02_write_byte(uint8_t addr, uint8_t data);
uint8_t at24c02_write_bytes(uint8_t addr, uint8_t* data, uint16_t len);
uint8_t at24c02_write_page(uint8_t addr, uint8_t *data);
uint8_t at24c02_read_bytes(uint8_t addr, uint8_t *data, uint16_t len);

#endif //AT24C02_H
