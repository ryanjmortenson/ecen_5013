/** @file mock_i2c.c
*
* @brief Mock I2C for workstation development
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>

#include "i2c.h"
#include "log.h"
#include "project_defs.h"

status_t i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t byte)
{
  FUNC_ENTRY;
  return SUCCESS;
}

status_t i2c_write_bytes(uint8_t addr, uint8_t reg, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  return SUCCESS;
}

status_t i2c_read_byte(uint8_t addr, uint8_t reg, uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  *byte = 12;
  return SUCCESS;
}

status_t i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  for (uint8_t i = 0; i < len; i++)
  {
    *(bytes + i) = 15;
  }
  return SUCCESS;
}
