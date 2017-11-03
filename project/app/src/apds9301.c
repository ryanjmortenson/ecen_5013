/** @file apds9301.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "apds9301.h"
#include "i2c.h"
#include "log.h"
#include "project_defs.h"

#define APDS9301_R_ADD (0x39)
#define BYTES_TO_ADC(bytes) (bytes[1] << 8 | bytes[0])

static i2c_descriptor_t * i2cd;

status_t apds9301_init(int32_t i2c_bus)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  command_reg_t cmd = COMMAND_INIT(ADDR_CONTROL, 0);
  do
  {
    if (i2c_init(i2c_bus, &i2cd, APDS9301_R_ADD) != SUCCESS)
    {
      LOG_ERROR("Could not initialize I2C for APDS9301");
      status = FAILURE;
      break;
    }

    if (apds9301_w_byte(cmd, 3) != SUCCESS)
    {
      LOG_ERROR("Could not turn on APDS9301 device");
      status = FAILURE;
      break;
    }
  } while (0);
  return status;
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

  if (i2c_write_byte(i2cd, cmd.reg) != SUCCESS)
  {
    LOG_ERROR("Could not write to pointer register");
    status = FAILURE;
  }
  return status;
}

inline static status_t apds9301_r_bytes(command_reg_t cmd, uint8_t * bytes, uint8_t len)
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

    res = i2c_read_bytes(i2cd, bytes, len);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not read apds9301 register");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t apds9301_r_byte(command_reg_t cmd, uint8_t * bytes)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  return apds9301_r_bytes(cmd, bytes, 1);
}

status_t apds9301_r_word(command_reg_t cmd, uint8_t * bytes)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  return apds9301_r_bytes(cmd, bytes, 2);
}

inline static status_t apds9301_w_bytes(command_reg_t cmd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  uint8_t new_bytes[3];
  int32_t res;
  status_t status = SUCCESS;

  new_bytes[0] = cmd.reg;
  memcpy(&new_bytes[1], bytes, len);

  res = i2c_write_bytes(i2cd, new_bytes, len + 1);
  if (res != SUCCESS)
  {
    LOG_ERROR("Could not write to apds9301 register");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_w_byte(command_reg_t cmd, uint8_t byte)
{
  FUNC_ENTRY;
  return apds9301_w_bytes(cmd, &byte, 1);
}

status_t apds9301_w_word(command_reg_t cmd, uint8_t * bytes)
{
  FUNC_ENTRY;
  return apds9301_w_bytes(cmd, bytes, 2);
}

inline static float calculate_lux(uint16_t adc0, uint16_t adc1)
{
  float conversion;
  float ratio = (float)adc1/(float)adc0;
  if (ratio > 0.0f && ratio <= .50f)
  {
    conversion = (0.0304f * adc0) - (.062f * adc0 * (pow(ratio, 1.4)));
  }
  else if (ratio > 0.50f && ratio <= .61f)
  {
    conversion = (0.0224f * adc0) - (0.031f * adc1);
  }
  else if (ratio > 0.61f && ratio <= .80f)
  {
    conversion = (0.0128f * adc0) - (0.0153f * adc1);
  }
  else if (ratio > 0.81f && ratio <= 1.30f)
  {
    conversion = (0.00146f * adc0) - (0.00112f * adc1);
  }
  else
  {
    conversion = 0.0f;
  }
  return conversion;
}
status_t apds9301_r_lux(float * lux)
{
  FUNC_ENTRY;
  CHECK_NULL(lux);
  uint8_t adc0_bytes[2];
  uint8_t adc1_bytes[2];
  command_reg_t cmd = COMMAND_INIT(ADDR_DATA0LOW, 1);
  status_t status = SUCCESS;

  do
  {
    if (apds9301_r_bytes(cmd, adc0_bytes, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read ADC0 value");
      status = FAILURE;
      break;
    }

    cmd.command.addr = ADDR_DATA1LOW;
    if (apds9301_r_bytes(cmd, adc1_bytes, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read ADC1 value");
      status = FAILURE;
      break;
    }
  } while (0);

  if (status == SUCCESS)
  {
    *lux =  calculate_lux(BYTES_TO_ADC(adc0_bytes), BYTES_TO_ADC(adc1_bytes));
  }
  return status;
}
