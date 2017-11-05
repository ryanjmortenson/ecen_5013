/** @file tmp102.h
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _TMP102_H
#define _TMP102_H

#include <stdint.h>

#include "project_defs.h"

typedef union {
    struct {
      uint16_t shutdown_mode    :1;
      uint16_t thermostat_mode  :1;
      uint16_t polarity         :1;
      uint16_t fault_queue      :2;
      uint16_t conv_resolution  :2;
      uint16_t one_shot_conv    :1;

      uint16_t reserved         :4;
      uint16_t ext_mode         :1;
      uint16_t alert            :1;
      uint16_t conv_rate        :2;
    } config;
    uint16_t reg;
} config_reg_t;

typedef enum pointer_reg {
  POINTER_TEMP = 0,
  POINTER_CONFIG,
  POINTER_TLOW,
  POINTER_THIGH
} pointer_reg_t;

/*!
* @brief Convert a temp reading
* @param[in] data data from I2C sensor
* @return converted temp
*/
float convert_temp(uint16_t data);

/*!
* @brief Initialize tmp102 sensor
* @param[in] BBB I2C bus sensor is on
* @return status of setting up tmp102
*/
status_t tmp102_init(int32_t i2c_bus);

/*!
* @brief Destroy tmp102 sensor
* @return status of destroying tmp102
*/
status_t tmp102_dest();

/*!
* @brief Generic read register bytes
* @param[in] reg which register to read
* @param[out] bytes read from register
* @return status of reading register
*/
status_t tmp102_r_reg(pointer_reg_t reg, uint8_t * bytes);

/*!
* @brief Generic write register bytes
* @param[in] reg which register to read
* @param[in] bytes to write from register
* @return status of writing register
*/
status_t tmp102_w_reg(pointer_reg_t reg, uint8_t * bytes);

/*!
* @brief Read config register
* @param[out] config read
* @return status of reading config
*/
status_t tmp102_r_cfg(config_reg_t * config);

/*!
* @brief Write config register
* @param[in] config to write
* @return status of writing config
*/
status_t tmp102_w_cfg(config_reg_t config);

/*!
* @brief Write pointer register
* @param[in] reg pointer to write
* @return status of writing pointer
*/
status_t tmp102_w_ptr(pointer_reg_t reg);

/*!
* @brief Read temp
* @param[out] tempc temperature
* @return status of reading temp
*/
status_t tmp102_r_tmp(float * tempc);

/*!
* @brief Set shutown mode
* @param[in] sd shutdown mode on or off
* @return status of setting shutdown mode
*/
status_t tmp102_sd_mode(uint8_t sd);

/*!
* @brief Set conversion resolution
* @param[in] cr conversion resolution to set
* @return status of setting conversion resolution
*/
status_t tmp102_set_cr(uint8_t sd);

#endif /* _TMP102_H */
