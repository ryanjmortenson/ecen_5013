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

status_t apds9301_r_reg(uint8_t reg, uint8_t * byte);
status_t apds9301_w_reg(uint8_t reg, uint8_t byte);

status_t apds9301_r_lux(uint8_t * byte);

#endif /* _APDS9301_H */
