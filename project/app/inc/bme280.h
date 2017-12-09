/** @file bme280.h
*
* @brief BME280 Functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _BME280_H
#define _BME280_H

#include <stdint.h>

#include "project_defs.h"

typedef enum bme280_addr {
  BME280_CTRL_HUM = 0xf2,
  BME280_CTRL_MEAS = 0xf4,
  BME280_CONFIG = 0xf5,
  BME280_HUM_MSB = 0xfd,
  BME280_HUM_LSB = 0xfe
} bme280_addr_t;

status_t bme280_init(int32_t i2c_bus);
status_t bme280_dest();
status_t bme280_w_reg(bme280_addr_t reg, uint8_t * bytes, uint8_t len);

#endif // _BME280_H
