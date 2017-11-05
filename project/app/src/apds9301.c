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
#define GAIN (16)

static i2c_descriptor_t * i2cd;

float scale_factor[INT_END] = {
  [INT_13_7_MS] = 0.034f,
  [INT_101_MS]  = 0.252f,
  [INT_402_MS]  = 1.0f
};

/*!
* @brief Convert ADC values to lux
* @param[in] adc0 reading
* @param[in] adc1 reading
* @return conversion
*/
inline static float calculate_lux(uint16_t adc0, uint16_t adc1, timing_reg_t * timing)
{
  float conversion;
  float ratio = (float)adc1/(float)adc0;
  float fadc0 = (float)adc0 * (float)1/scale_factor[timing->timing.integration];
  float fadc1 = (float)adc1 * (float)1/scale_factor[timing->timing.integration];

  if (!timing->timing.gain)
  {
    fadc0 *= GAIN;
    fadc1 *= GAIN;
  }

  if (ratio <= .50f)
  {
    conversion = (0.0304f * fadc0) - (.062f * fadc0 * (pow(ratio, 1.4)));
  }
  else if (ratio <= .61f)
  {
    conversion = (0.0224f * fadc0) - (0.031f * fadc1);
  }
  else if (ratio <= .80f)
  {
    conversion = (0.0128f * fadc0) - (0.0153f * fadc1);
  }
  else if (ratio <= 1.30f)
  {
    conversion = (0.00146f * fadc0) - (0.00112f * fadc1);
  }
  else
  {
    conversion = 0.0f;
  }
  return conversion;
}

/*!
* @brief Reads a number of bytes
* @param[in] cmd command word
* @param[out] bytes read
* @param[in] len number of bytes to read
* @return status of read
*/
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

/*!
* @brief Writes a number of bytes
* @param[in] cmd command word
* @param[in] bytes to write
* @param[in] len number of bytes to write
* @return status of write
*/
inline static status_t apds9301_w_bytes(command_reg_t cmd, uint8_t * bytes, uint8_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
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

status_t apds9301_r_lux(float * lux)
{
  FUNC_ENTRY;
  CHECK_NULL(lux);
  uint8_t adc0_bytes[2];
  uint8_t adc1_bytes[2];
  command_reg_t cmd = COMMAND_INIT(ADDR_DATA0LOW, 1);
  timing_reg_t timing;
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

    cmd.command.addr = ADDR_TIMING;
    if (apds9301_r_byte(cmd, &timing.reg) != SUCCESS)
    {
      LOG_ERROR("Could not read timing value");
      status = FAILURE;
      break;
    }
  } while (0);

  if (status == SUCCESS)
  {
    *lux =  calculate_lux(BYTES_TO_ADC(adc0_bytes), BYTES_TO_ADC(adc1_bytes), &timing);
  }
  return status;
}

status_t apds9301_r_ctrl(control_reg_t * control)
{
  FUNC_ENTRY;
  command_reg_t cmd = COMMAND_INIT(ADDR_CONTROL, 1);
  status_t status = SUCCESS;

  if (apds9301_r_byte(cmd, (uint8_t *)&control->reg) != SUCCESS)
  {
    LOG_ERROR("Could not read control reg");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_w_ctrl(control_reg_t control)
{
  FUNC_ENTRY;
  command_reg_t cmd = COMMAND_INIT(ADDR_TIMING, 1);
  status_t status = SUCCESS;

  if (apds9301_w_byte(cmd, control.reg) != SUCCESS)
  {
    LOG_ERROR("Could not write control reg");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_w_timing(timing_reg_t timing)
{
  FUNC_ENTRY;
  command_reg_t cmd = COMMAND_INIT(ADDR_TIMING, 1);
  status_t status = SUCCESS;

  if (apds9301_w_byte(cmd, timing.reg) != SUCCESS)
  {
    LOG_ERROR("Could not write timing reg");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_w_inter_ctrl(interrupt_ctrl_reg_t intr)
{
  FUNC_ENTRY;
  command_reg_t cmd = COMMAND_INIT(ADDR_INTERRUPT, 1);
  status_t status = SUCCESS;

  if (apds9301_w_byte(cmd, intr.reg)  != SUCCESS)
  {
    LOG_ERROR("Could not write interrupt control reg");
    status = FAILURE;
  }
  return status;
}

status_t apds9301_r_id(id_reg_t * id)
{
  FUNC_ENTRY;
  command_reg_t cmd = COMMAND_INIT(ADDR_ID, 1);
  status_t status = SUCCESS;

  if (apds9301_r_byte(cmd, (uint8_t *)&id->reg) != SUCCESS)
  {
    LOG_ERROR("Could not read id reg");
    status = FAILURE;
  }
  return status;
}
