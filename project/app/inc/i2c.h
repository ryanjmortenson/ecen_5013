/** @file i2c.h
*
* @brief I2C functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _I2C_H
#define _I2C_H

#include <stdint.h>
#include "project_defs.h"

/*!
* @brief Writes a single byte
* @param addr address of i2c device
* @param reg register to write byte to
* @param byte byte to write
* @return status of write
*/
status_t i2c_write_byte(uint8_t addr, uint8_t reg, uint8_t byte);

/*!
* @brief Writes multiple bytes
* @param addr address of i2c device
* @param reg register to start writing bytes to
* @param byte pointer to buffer of bytes to write
* @param len number of bytes
* @return status of write
*/
status_t i2c_write_bytes(uint8_t addr, uint8_t reg, uint8_t * bytes, uint8_t len);

/*!
* @brief Reads a single byte
* @param addr address of i2c device
* @param reg register to read by from
* @param byte pointer to location to store byte
* @return status of read
*/
status_t i2c_read_byte(uint8_t addr, uint8_t reg, uint8_t * byte);

/*!
* @brief Reads multiple bytes
* @param addr address of i2c device
* @param reg register to start reading bytes from
* @param bytes pointer to location to store bytes
* @param len number of bytes to read
* @return status of read
*/
status_t i2c_read_bytes(uint8_t addr, uint8_t reg, uint8_t * bytes, uint8_t len);

#endif /* _I2C_H */
