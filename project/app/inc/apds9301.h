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

/*!
* @brief Initialize apds9301 sensor
* @param[in] BBB I2C bus sensor is on
* @return status of setting up apds9301
*/
status_t apds9301_init(int32_t i2c_bus);

/*!
* @brief Destroy apds9301 sensor
* @return status of destroying apds9301
*/
status_t apds9301_dest();

/*!
* @brief Generic read byte
* @param[in] cmd which register to read
* @param[out] byte read from register
* @return status of reading byte
*/
status_t apds9301_r_byte(command_reg_t cmd, uint8_t * byte);

/*!
* @brief Generic read word
* @param[in] cmd which register to read
* @param[out] bytes read from register
* @return status of reading word
*/
status_t apds9301_r_word(command_reg_t cmd, uint8_t * bytes);

/*!
* @brief Generic write byte
* @param[in] cmd which register to write
* @param[in] byte to write into register
* @return status of writing byte
*/
status_t apds9301_w_byte(command_reg_t cmd, uint8_t byte);

/*!
* @brief Generic write bytes
* @param[in] cmd which register to write
* @param[in] bytes to write into register
* @return status of writing bytes
*/
status_t apds9301_w_word(command_reg_t cmd, uint8_t * bytes);

/*!
* @brief Read lux
* @param[out] lux reading
* @return status reading lux
*/
status_t apds9301_r_lux(float * lux);

// Static initializer for command structure
#define COMMAND_INIT(_addr, _word) {.command.addr  = _addr,   \
                                    .command.word  = _word,   \
                                    .command.clear = 0,       \
                                    .command.cmd   = 1}
#endif /* _APDS9301_H */
