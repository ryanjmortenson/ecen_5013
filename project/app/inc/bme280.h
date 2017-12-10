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
  BME280_HUM_LSB = 0xfe,
  BME280_DIG_H1  = 0xa1,
  BME280_DIG_H2  = 0xe1,
  BME280_DIG_H3  = 0xe3,
  BME280_DIG_E4  = 0xe4,
  BME280_DIG_E5  = 0xe5,
  BME280_DIG_E6  = 0xe6,
  BME280_DIG_H6  = 0xe7,
  BME280_TEMP_MSB = 0xfa,
  BME280_TEMP_XLSB = 0xfc,
  BME280_DIG_T1  = 0x88,
  BME280_DIG_T2  = 0x8a,
  BME280_DIG_T3  = 0x8c
} bme280_addr_t;

/*!
* @brief Initialize bme280 module
* @param i2c_bus the i2c bus being used
* @return status of initializing bme280 module
*/
status_t bme280_init(int32_t i2c_bus);

/*!
* @brief Destroy bme280 module
* @return status of destroying bme280
*/
status_t bme280_dest();

/*!
* @brief Generic write register bytes
* @param[in] reg which register to write
* @param[in] bytes to write from register
* @param[in] len num bytes to write
* @return status of writing register
*/
status_t bme280_w_reg(bme280_addr_t reg, uint8_t * bytes, uint8_t len);

/*!
* @brief Generic read register bytes
* @param[in] reg which register to read
* @param[out] bytes place to store bytes read from register
* @param[in] len num bytes to read
* @return status of reading register
*/
status_t bme280_r_reg(bme280_addr_t reg, uint8_t * bytes, uint8_t len);

/*!
* @brief Read humidity
* @param[out] humidity output
* @return status of reading humidity
*/
status_t bme280_r_humidity(uint16_t * humidity);

#endif // _BME280_H
