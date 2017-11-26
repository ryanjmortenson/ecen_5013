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

status_t ccs811_init(int32_t i2c_bus);
status_t ccs811_dest();
status_t ccs811_w_reg(ccs811_addr_t reg, uint8_t * bytes, uint8_t len);
status_t ccs811_start_app();
#endif // _CCS811_H
