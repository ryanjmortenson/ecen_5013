/** @file tmp102.c
*
* @brief TMP102 functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>

#include "i2c.h"
#include "log.h"
#include "project_defs.h"
#include "tmp102.h"

#define TMP102_ADD (0x12)
#define TMP102_TEMP_REG (0x12)

status_t tmp102_r_reg(uint8_t reg, uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  return i2c_read_byte(TMP102_ADD, reg, byte);
}

status_t tmp102_w_reg(uint8_t reg, uint8_t byte)
{
  FUNC_ENTRY;
  return i2c_write_byte(TMP102_ADD, reg, byte);
}

status_t tmp102_r_tmp(uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  return tmp102_r_reg(TMP102_TEMP_REG, byte);
}

