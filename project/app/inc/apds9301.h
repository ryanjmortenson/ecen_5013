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

typedef enum integration_time {
  INT_13_7_MS,
  INT_101_MS,
  INT_402_MS,
  INT_END
} integration_time_t;

// Register bit fields
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

typedef union control_reg {
  struct {
    uint8_t power     :2;
    uint8_t reserved  :6;
  } control;
  uint8_t reg;
} control_reg_t;

typedef union timing_reg {
  struct {
    uint8_t integration :2;
    uint8_t reserved    :1;
    uint8_t manual      :1;
    uint8_t gain        :1;
    uint8_t reserverd2  :3;
  } timing;
  uint8_t reg;
} timing_reg_t;

typedef union interrupt_thresh_reg {
  struct {
    uint32_t threshlowlow   :8;
    uint32_t threshlowhigh  :8;
    uint32_t threshhighlow  :8;
    uint32_t threshhighhigh :8;
  } interrupt_thresh;
  uint32_t regs;
} interrupt_thresh_reg_t;

typedef union interrupt_ctrl_reg {
  struct {
    uint8_t persist    :4;
    uint8_t intr       :2;
    uint8_t reserved   :2;
  } interrupt_ctrl;
  uint8_t reg;
} interrupt_ctrl_reg_t;

typedef union id_reg {
  struct {
    uint8_t revno      :4;
    uint8_t partnum    :4;
  } id;
  uint8_t reg;
} id_reg_t;

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
* @brief Write command register
* @param[in] command register
* @return status writing command register
*/
status_t apds9301_w_cmd(command_reg_t cmd);

/*!
* @brief Read lux
* @param[out] lux reading
* @return status reading lux
*/
status_t apds9301_r_lux(float * lux);

/*!
* @brief Read control register
* @param[out] control register
* @return status reading control
*/
status_t apds9301_r_ctrl(control_reg_t * control);

/*!
* @brief Write control register
* @param[in] control register
* @return status writing control
*/
status_t apds9301_w_ctrl(control_reg_t control);

/*!
* @brief Write timing register
* @param[in] timing register
* @return status writing timing
*/
status_t apds9301_w_timing(timing_reg_t timing);

/*!
* @brief Write interrupt control register
* @param[in] interrupt control register
* @return status writing interrupt control
*/
status_t apds9301_w_inter_ctrl(interrupt_ctrl_reg_t intr);

/*!
* @brief Read id register
* @param[out] id register
* @return status reading id
*/
status_t apds9301_r_id(id_reg_t * id);

// Static initializer for command structure
#define COMMAND_INIT(_addr, _word) {.command.addr  = _addr,   \
                                    .command.word  = _word,   \
                                    .command.clear = 0,       \
                                    .command.cmd   = 1}
#endif /* _APDS9301_H */
