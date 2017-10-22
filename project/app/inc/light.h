/** @file light.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _LIGHT_H
#define _LIGHT_H

/*!
* @brief Initialize light task
* @return status of initializing light
*/
status_t init_light();

/*!
* @brief Destroy light task
* @return status of destroying light
*/
status_t dest_light();

/*!
* @brief Determine if it is dark
* @return status of making call
*/
status_t is_dark();

#endif /* _LIGHT_H */
