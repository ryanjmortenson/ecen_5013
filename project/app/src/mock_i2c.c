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

struct i2c_descriptor {
  int32_t addr;
};

i2c_descriptor_t desc;

status_t i2c_init(int32_t i2c_bus, i2c_descriptor_t ** i2cd, uint8_t addr)
{
  FUNC_ENTRY;
  *i2cd = &desc;
  return SUCCESS;
}

status_t i2c_write_byte(i2c_descriptor_t * i2cd, uint8_t byte)
{
  FUNC_ENTRY;
  return SUCCESS;
}

status_t i2c_write_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  return SUCCESS;
}

status_t i2c_read_byte(i2c_descriptor_t * i2cd, uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  *byte = 12;
  return SUCCESS;
}

status_t i2c_read_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  for (uint8_t i = 0; i < len; i++)
  {
    *(bytes + i) = 15;
  }
  return SUCCESS;
}

status_t i2c_dest(i2c_descriptor_t * i2cd)
{
  FUNC_ENTRY;
  return SUCCESS;
}
