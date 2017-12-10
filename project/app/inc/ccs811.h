/** @file ccs811.h
*
* @brief CCS811 Functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _CCS811_H
#define _CCS811_H

#include <stdint.h>
#include "project_defs.h"

// Register addresses
typedef enum ccs811_addr {
  CCS811_STATUS,
  CCS811_MEASURE,
  CCS811_ALGORITHM,
  CCS811_RAW,
  CCS811_ENV           = 0x05,
  CCS811_NTC,
  CCS811_THRESHOLD     = 0x10,
  CCS811_BASE,
  CCS811_HIGH          = 0x20,
  CCS811_HV,
  CCS811_BOOT          = 0x23,
  CCS811_APPV,
  CCS811_ERR           = 0xE0,
  CCS811_APPS          = 0xF4,
  CCS811_RESET         = 0xFF
} ccs811_addr_t;

/*!
* @brief Initialize ccs811 module
* @param i2c_bus the i2c bus being used
* @return status of initializing ccs811 module
*/
status_t ccs811_init(int32_t i2c_bus);

/*!
* @brief Destroy ccs811 module
* @return status of destroying ccs811
*/
status_t ccs811_dest();

/*!
* @brief Generic write register bytes
* @param[in] reg which register to write
* @param[in] bytes to write from register
* @param[in] len num bytes to write
* @return status of writing register
*/
status_t ccs811_w_reg(ccs811_addr_t reg, uint8_t * bytes, uint8_t len);


/*!
* @brief Start the measurement application
* @return status of starting app
*/
status_t ccs811_start_app();

/*!
* @brief Read tvoc
* @param[out] tvoc output
* @return status of reading tvoc
*/
status_t ccs811_r_tvoc(uint16_t * tvoc);

/*!
* @brief Read co2
* @param[out] co2 output
* @return status of reading co2
*/
status_t ccs811_r_co2(uint16_t * co2);
#endif // _CCS811_H
