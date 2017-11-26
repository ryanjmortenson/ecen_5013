/** @file ccs811.c
*
* @brief CCS811 functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <string.h>

#include "i2c.h"
#include "log.h"
#include "project_defs.h"
#include "ccs811.h"

#define CCS811_R_ADD (0x5b)
#define WRITE_MAX (8)

static i2c_descriptor_t * i2cd;

status_t ccs811_init(int32_t i2c_bus)
{
  FUNC_ENTRY;
  ccs811_start_app();
  return i2c_init(i2c_bus, &i2cd, CCS811_R_ADD);
}

status_t ccs811_dest()
{
  FUNC_ENTRY;
  return i2c_dest(i2cd);
}

status_t ccs811_r_reg(ccs811_addr_t reg, uint8_t * bytes, uint8_t len)
{
  CHECK_NULL(bytes);
  status_t status = SUCCESS;

  if (i2c_write_byte(i2cd, (uint8_t)reg) != SUCCESS)
  {
    LOG_ERROR("Could not write reg");
    status = FAILURE;
  }

  if (i2c_read_bytes(i2cd, bytes, len) != SUCCESS)
  {
    LOG_ERROR("Could not write reg");
    status = FAILURE;
  }
  return status;
}

status_t ccs811_w_reg(ccs811_addr_t reg, uint8_t * bytes, uint8_t len)
{
  CHECK_NULL(bytes);
  uint8_t new_bytes[WRITE_MAX];
  int32_t res;
  status_t status = SUCCESS;

  if (len < WRITE_MAX - 1)
  {
    new_bytes[0] = (uint8_t)reg;
    memcpy(&new_bytes, bytes, len);

    res = i2c_write_bytes(i2cd, new_bytes, 3);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not write to ccs811 register");
      status = FAILURE;
    }
  }
  else
  {
    status = FAILURE;
  }
  return status;
}

status_t ccs811_start_app()
{
  uint8_t start_app = 0x10;
  return ccs811_w_reg(CCS811_APPS, &start_app, 1);
}
