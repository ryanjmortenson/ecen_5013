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

typedef struct i2c_descriptor i2c_descriptor_t;

/*!
* @brief Initialize i2c bus, should be in proprietary folder
* @param i2c_bus bus number to use for i2c
* @param i2cd i2c descriptor
* @return status of initialization
*/
status_t i2c_init(int32_t i2c_bus, i2c_descriptor_t ** i2cd, uint8_t addr);

/*!
* @brief Initialize i2c bus, should be in proprietary folder
* @param i2c_bus bus number to use for i2c
* @param i2cd i2c descriptor
* @return status of initialization
*/
status_t i2c_dest(i2c_descriptor_t * i2cd);
/*!
* @brief Writes a single byte
* @param addr address of i2c device
* @param byte byte to write
* @return status of write
*/
status_t i2c_write_byte(i2c_descriptor_t * i2cd, uint8_t byte);

/*!
* @brief Writes multiple bytes
* @param addr address of i2c device
* @param byte pointer to buffer of bytes to write
* @param len number of bytes
* @return status of write
*/
status_t i2c_write_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len);

/*!
* @brief Reads a single byte
* @param addr address of i2c device
* @param byte pointer to location to store byte
* @return status of read
*/
status_t i2c_read_byte(i2c_descriptor_t * i2cd, uint8_t * byte);

/*!
* @brief Reads multiple bytes
* @param addr address of i2c device
* @param bytes pointer to location to store bytes
* @param len number of bytes to read
* @return status of read
*/
status_t i2c_read_bytes(i2c_descriptor_t * i2cd, uint8_t * bytes, uint8_t len);

#endif /* _I2C_H */
