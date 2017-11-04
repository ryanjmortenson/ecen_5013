/** @file tmp102.c
*
* @brief TMP102 functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <string.h>

#include "i2c.h"
#include "log.h"
#include "project_defs.h"
#include "tmp102.h"

#define TMP102_R_ADD (0x49)
#define TMP102_TEMP_REG (0x12)
#define TMP102_READ_CMD_LEN (3)
#define TMP102_READ_DATA_LEN (2)

static i2c_descriptor_t * i2cd;

status_t tmp102_init(int32_t i2c_bus)
{
  FUNC_ENTRY;
  uint16_t byte = 0xffff;
  i2c_init(i2c_bus, &i2cd, TMP102_R_ADD);
  tmp102_w_reg(POINTER_TLOW, (uint8_t *)&byte);
  return SUCCESS;
}

status_t tmp102_dest()
{
  FUNC_ENTRY;
  return i2c_dest(i2cd);
}

status_t tmp102_w_ptr(pointer_reg_t reg)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;

  if (i2c_write_byte(i2cd, (uint8_t)reg) != SUCCESS)
  {
    LOG_ERROR("Could not write to pointer register");
    status = FAILURE;
  }
  return status;
}

status_t tmp102_w_reg(pointer_reg_t reg, uint8_t * bytes)
{
  FUNC_ENTRY;
  uint8_t new_bytes[3];
  int32_t res;
  status_t status = SUCCESS;

  new_bytes[0] = (uint8_t)reg;
  memcpy(&new_bytes[1], bytes, 2);

  res = i2c_write_bytes(i2cd, new_bytes, 3);
  if (res != SUCCESS)
  {
    LOG_ERROR("Could not write to tmp102 register");
    status = FAILURE;
  }
  return status;
}

status_t tmp102_r_reg(pointer_reg_t reg, uint8_t * bytes)
{
  FUNC_ENTRY;
  CHECK_NULL(bytes);
  status_t status = SUCCESS;
  int32_t res;

  do
  {
    res = tmp102_w_ptr(reg);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not write to tmp102 register");
      status = FAILURE;
      break;
    }

    res = i2c_read_bytes(i2cd, bytes, TMP102_READ_DATA_LEN);
    if (res != SUCCESS)
    {
      LOG_ERROR("Could not write to tmp102 register");
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

status_t tmp102_r_tmp(float * tempc)
{
  FUNC_ENTRY;
  CHECK_NULL(tempc);
  uint8_t bytes[TMP102_READ_DATA_LEN] = {0};
  uint16_t data = 0;

  status_t status = tmp102_r_reg(POINTER_TEMP, bytes);
  if (status == SUCCESS)
  {
    data = (bytes[0] << 4) | (bytes[1] >> 4);
    *tempc = (float)(data) * 0.0625f;
    LOG_LOW("Calculated temp %fC", *tempc);
  }
  else
  {
    LOG_ERROR("Could not read temperature");
  }
  return status;
}

status_t tmp102_r_cfg(config_reg_t * config)
{
  FUNC_ENTRY;

  status_t status = tmp102_r_reg(POINTER_CONFIG, (uint8_t *)&config->reg);
  if (status == SUCCESS)
  {
    LOG_LOW("Configuration registers 0x%04x", config->reg);
  }
  else
  {
    LOG_ERROR("Could not read config");
  }
  return status;
}

status_t tmp102_w_cfg(config_reg_t config)
{
  FUNC_ENTRY;
  status_t status;

  status = tmp102_w_reg(POINTER_CONFIG, (uint8_t *)&config.reg);
  if (status == FAILURE)
  {
    LOG_ERROR("Could not write config");
  }
  return status;
}

status_t tmp102_sd_mode(uint8_t sd)
{
  FUNC_ENTRY;
  config_reg_t cfg = {0};
  status_t status = SUCCESS;

  do
  {
    if (tmp102_r_cfg(&cfg) != SUCCESS)
    {
      status = FAILURE;
      break;
    }

    cfg.config.shutdown_mode = sd ? 1 : 0;
    if (tmp102_w_cfg(cfg) != SUCCESS)
    {
      status = FAILURE;
      break;
    }
  } while (0);
  return status;
}

status_t tmp102_set_cr(uint8_t cr)
{
  FUNC_ENTRY;
  config_reg_t cfg = {0};
  status_t status = SUCCESS;

  do
  {
    if (tmp102_r_cfg(&cfg) != SUCCESS)
    {
      status = FAILURE;
      break;
    }

    cfg.config.conv_resolution = cr;
    if (tmp102_w_cfg(cfg) != SUCCESS)
    {
      status = FAILURE;
      break;
    }
  } while (0);
  return status;
}
