/** @file apds9301.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>

#include "apds9301.h"
#include "i2c.h"
#include "log.h"
#include "project_defs.h"

#define APDS9301_R_ADD (0x39)

static i2c_descriptor_t * i2cd;

status_t apds9301_init(int32_t i2c_bus)
{
  FUNC_ENTRY;
  return i2c_init(i2c_bus, &i2cd, APDS9301_R_ADD);
}

status_t apds9301_dest()
{
  FUNC_ENTRY;
  return i2c_dest(i2cd);
}

status_t apds9301_w_cmd(command_reg_t cmd)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  command_reg_t command = COMMAND_INIT(ADDR_ID, 0, 0, 1);

  if (i2c_write_byte(i2cd, command.reg) != SUCCESS)
  {
    LOG_ERROR("Could not write to pointer register");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_r_reg(command_reg_t cmd, uint8_t * bytes)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  status_t status = SUCCESS;
  int32_t res;

  do
  {
    res = apds9301_w_cmd(cmd);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not write to apds9301 command register");
      status = FAILURE;
      break;
    }

    res = i2c_read_bytes(i2cd, bytes, 1);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not read apds9301 register");
      status = FAILURE;
      break;
    }
    else
    {
      LOG_LOW("Raw data 0: 0x%02x, 1: 0x%02x", bytes[0], bytes[1]);
    }
  } while(0);
  return status;

}

status_t apds9301_w_reg(uint8_t reg, uint8_t byte)
{
  FUNC_ENTRY;
  return SUCCESS;
}

status_t apds9301_r_lux(uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  return SUCCESS;
}
