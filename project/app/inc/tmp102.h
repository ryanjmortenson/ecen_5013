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

status_t tmp102_init(int32_t i2c_bus);
status_t tmp102_r_reg(pointer_reg_t reg, uint8_t * bytes);
status_t tmp102_w_reg(pointer_reg_t reg, uint8_t * bytes);

status_t tmp102_w_cfg(config_reg_t * config);
status_t tmp102_r_cfg(config_reg_t * config);
status_t tmp102_w_ptr(pointer_reg_t reg);
status_t tmp102_r_tmp(float * tempc);

#endif /* _TMP102_H */
