/** @file bme280.c
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
#include "bme280.h"

#define BME280_R_ADDR (0x77)
#define WRITE_MAX (8)

static i2c_descriptor_t * i2cd;

struct {
 uint8_t h1_val;
 int16_t h2_val;
 uint8_t h3_val;
 int8_t  h4_val;
 int8_t  h5_val;
 int8_t  h6_val;
 int8_t  e5_val;
 uint16_t t1_val;
 int16_t  t2_val;
 int16_t  t3_val;
 uint32_t t_fine;
} comps;

uint8_t hum_setting = 0x01;
uint8_t meas_setting = 0x27;
uint8_t config_setting = 0;

static inline uint32_t comp_temp(uint32_t temp)
{
    uint32_t first = (((temp >> 3) - (comps.t1_val << 1)) * comps.t2_val) >> 11;
    uint32_t second = (((((temp >> 4) - comps.t1_val) * ((temp >> 4) - (comps.t1_val))) >> 12) * (comps.t3_val)) >> 14;
    comps.t_fine = first + second;
    return (comps.t_fine * 5 + 128) >> 8;
}

static inline uint32_t comp_hum(uint32_t humidity)
{
  uint32_t hum = (comps.t_fine - 76800);
  hum = (((((humidity << 14) - (comps.h4_val << 20) - (comps.h5_val * hum)) + 16384) >> 15) * (((((((hum * comps.h6_val) >> 10) * (((hum * comps.h3_val) >> 11) + 32768)) >> 10) + 2097152) * comps.h2_val + 8192) >> 14));
  hum = hum - (((((hum >> 15) * (hum >> 15)) >> 7) * comps.h1_val) >> 4);
  hum = (hum < 0 ? 0 : hum);
  hum = (hum > 419430400 ? 419430400 : hum);
  return (hum >> 12);
}

status_t bme280_init(int32_t i2c_bus)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  do
  {
    if (i2c_init(i2c_bus, &i2cd, BME280_R_ADDR) != SUCCESS)
    {
      LOG_ERROR("Could not initialize I2C");
      status = FAILURE;
      break;
    }

    if (bme280_w_reg(BME280_CTRL_HUM, &hum_setting, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_w_reg(BME280_CTRL_MEAS, &meas_setting, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_w_reg(BME280_CONFIG, &config_setting, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_E5, (uint8_t *)&comps.e5_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_H1, (uint8_t *)&comps.h1_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_H2, (uint8_t *)&comps.h2_val, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_H3, (uint8_t *)&comps.h3_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_E4, (uint8_t *)&comps.h4_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }
    comps.h4_val = (comps.h4_val << 4) | (comps.e5_val & 0xf);

    if (bme280_r_reg(BME280_DIG_E6, (uint8_t *)&comps.h5_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }
    comps.h5_val = (comps.h5_val << 4) | (comps.e5_val >> 4);

    if (bme280_r_reg(BME280_DIG_H6, (uint8_t *)&comps.h6_val, 1) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_T1, (uint8_t *)&comps.t1_val, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_T2, (uint8_t *)&comps.t2_val, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }

    if (bme280_r_reg(BME280_DIG_T3, (uint8_t *)&comps.t3_val, 2) != SUCCESS)
    {
      LOG_ERROR("Could not read E5 comp");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t bme280_dest()
{
  FUNC_ENTRY;
  return i2c_dest(i2cd);
}

status_t bme280_r_reg(bme280_addr_t reg, uint8_t * bytes, uint8_t len)
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

status_t bme280_w_reg(bme280_addr_t reg, uint8_t * bytes, uint8_t len)
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
      LOG_ERROR("Could not write to bme280 register");
      status = FAILURE;
    }
  }
  else
  {
    status = FAILURE;
  }
  return status;
}

status_t bme280_r_temp(uint32_t * temp)
{
  status_t status = SUCCESS;
  uint16_t temp_msb;
  uint8_t temp_xlsb;

  if (bme280_r_reg(BME280_TEMP_MSB, (uint8_t *)&temp_msb, 2) != SUCCESS)
  {
    LOG_ERROR("Could not read temp msb");
    status = FAILURE;
  }

  if (bme280_r_reg(BME280_TEMP_XLSB, (uint8_t *)&temp_xlsb, 1) != SUCCESS)
  {
    LOG_ERROR("Could not read xlsb");
    status = FAILURE;
  }
  *temp = comp_temp((temp_msb << 4) | (temp_xlsb >> 4));
  return status;
}

status_t bme280_r_humidity(uint16_t * humidity)
{
  status_t status = SUCCESS;
  uint32_t temp;

  // Read the temp to get the most accurate t fine value
  bme280_r_temp(&temp);
  if (bme280_r_reg(BME280_HUM_MSB, (uint8_t *)humidity, 2) != SUCCESS)
  {
    LOG_ERROR("Could not read humidity");
    status = FAILURE;
  }
  *humidity = comp_hum(*humidity);
  return status;
}
