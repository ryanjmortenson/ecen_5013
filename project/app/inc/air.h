/** @file air.h
*
* @brief Air task public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _AIR_H
#define _AIR_H

#include "project_defs.h"
#include "workers.h"

/*!
* @brief Initialize air task
* @return status of initializing air
*/
status_t init_air();

/*!
* @brief Destroy air task
* @return status of destroying air
*/
status_t dest_air();

#endif /* _AIR_H */
