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

#define APDS9301_ADD (0x13)

status_t apds9301_r_reg(uint8_t reg, uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  return SUCCESS;
  // return i2c_read_byte(APDS9301_ADD, reg, byte);
}

status_t apds9301_w_reg(uint8_t reg, uint8_t byte)
{
  FUNC_ENTRY;
  return SUCCESS;
  //return i2c_write_byte(APDS9301_ADD, reg, byte);
}

status_t apds9301_r_lux(uint8_t * byte)
{
  FUNC_ENTRY;
  CHECK_NULL(byte);
  return SUCCESS;
  // return apds9301_r_reg(1, byte);
}
