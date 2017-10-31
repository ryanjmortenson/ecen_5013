/** @file apds9301.h
*
* @brief APDS9301 Functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _APDS9301_H
#define _APDS9301_H

#include <stdint.h>

#include "project_defs.h"

typedef enum apds_addr {
  ADDR_CONTROL = 0,
  ADDR_TIMING,
  ADDR_THRESHLOWLOW,
  ADDR_THRESHLOWHIGH,
  ADDR_THRESHHIGHLOW,
  ADDR_THRESHHIGHHIGH,
  ADDR_INTERRUPT,
  ADDR_ID = 10,
  ADDR_DATA0LOW = 12,
  ADDR_DATA0HIGH,
  ADDR_DATA1LOW,
  ADDR_DATA1HIGH
} apds_addr_t;

typedef union command_reg {
  struct {
    uint8_t addr      :4;
    uint8_t reserved  :1;
    uint8_t word      :1;
    uint8_t clear     :1;
    uint8_t cmd       :1;
  } command;
  uint8_t reg;
} command_reg_t;

status_t apds9301_init(int32_t i2c_bus);
status_t apds9301_dest();
status_t apds9301_r_reg(command_reg_t cmd, uint8_t * byte);
status_t apds9301_w_reg(uint8_t reg, uint8_t byte);

status_t apds9301_r_lux(uint8_t * byte);

#define COMMAND_INIT(_addr, _word, _clear, _cmd) {.command.addr = _addr,   \
                                                  .command.word = _word,   \
                                                  .command.clear = _clear, \
                                                  .command.cmd = _cmd}
#endif /* _APDS9301_H */
