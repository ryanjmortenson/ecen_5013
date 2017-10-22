/** @file i2c.h
*
* @brief I2C public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _I2C_H
#define _I2C_H

/*!
* @brief Initialize i2c task
* @return status of initializing i2c
*/
status_t init_i2c();

/*!
* @brief Destroy i2c task
* @return status of destroying i2c
*/
status_t dest_i2c();

/*!
* @brief Read register with i2c
* @return status of making call
*/
status_t i2c_read_reg();

/*!
* @brief Read registers with i2c
* @return status of making call
*/
status_t i2c_read_regs();

/*!
* @brief Write register with i2c
* @return status of making call
*/
status_t i2c_write_reg();

/*!
* @brief Write registers with i2c
* @return status of making call
*/
status_t i2c_write_regs();

#endif /* _I2C_H */
