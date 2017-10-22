/** @file temp.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _TEMP_H
#define _TEMP_H

/*!
* @brief Initialize temperature task
* @return status of initializing temp
*/
status_t init_temp();

/*!
* @brief Destroy temperature task
* @return status of destroying temp
*/
status_t dest_temp();

/*!
* @brief Get temperature in degrees celsius
* @return status of making call
*/
status_t get_c_temp();

/*!
* @brief Get temperature in kelvin
* @return status of making call
*/
status_t get_k_temp();

/*!
* @brief Get temperature in degrees farenheit
* @return status of making call
*/
status_t get_f_temp();

#endif /* _TEMP_H */
