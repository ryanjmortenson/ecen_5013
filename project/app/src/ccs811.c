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
  status_t status = SUCCESS;
  do
  {
    if (i2c_init(i2c_bus, &i2cd, CCS811_R_ADD) != SUCCESS)
    {
      LOG_ERROR("Could not initialize I2C");
      status = FAILURE;
      break;
    }

    if (ccs811_start_app() != SUCCESS)
    {
      LOG_ERROR("Could not start CCS811 app");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
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
    memcpy(&new_bytes[1], bytes, len);

    res = i2c_write_bytes(i2cd, new_bytes, len + 1);
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

status_t ccs811_r_alg(uint8_t * bytes)
{
  CHECK_NULL(bytes);
  const uint8_t ALG_LEN = 4;
  status_t status = SUCCESS;

  if (ccs811_r_reg(CCS811_ALGORITHM, bytes, ALG_LEN) != SUCCESS)
  {
    LOG_ERROR("Could not read algorithm register");
    status = FAILURE;
  }
  return status;
}

status_t ccs811_r_tvoc(uint16_t * tvoc)
{
  CHECK_NULL(tvoc);
  uint32_t bytes;
  status_t status = SUCCESS;

  if (ccs811_r_alg((uint8_t *)&bytes) != SUCCESS)
  {
    LOG_ERROR("Could not read algorithm register");
    status = FAILURE;
  }
  *tvoc = (status == SUCCESS) ? bytes & 0x0000ffff : 0;
  return status;
}

status_t ccs811_r_co2(uint16_t * co2)
{
  CHECK_NULL(co2);
  uint32_t bytes;
  status_t status = SUCCESS;

  if (ccs811_r_alg((uint8_t *)&bytes) != SUCCESS)
  {
    LOG_ERROR("Could not read algorithm register");
    status = FAILURE;
  }
  *co2 = (status == SUCCESS) ? (bytes & 0xffff0000) >> 16 : 0;
  return status;
}

status_t ccs811_start_app()
{
  uint8_t constant_mode = 0x10;
  ccs811_w_reg(CCS811_APPS, &constant_mode, 0);
  return ccs811_w_reg(CCS811_MEASURE, &constant_mode, 1);
}
