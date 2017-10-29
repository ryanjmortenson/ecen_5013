/** @file tmp102.h
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _TMP102_H
#define _TMP102_H

#include <stdint.h>

#include "project_defs.h"

status_t tmp102_r_reg(uint8_t reg, uint8_t * byte);
status_t tmp102_w_reg(uint8_t reg, uint8_t byte);

status_t tmp102_w_cfg(uint8_t byte);
status_t tmp102_r_cfg(uint8_t * byte);
status_t tmp102_r_ptr(uint8_t * byte);
status_t tmp102_r_tmp(uint8_t * byte);

#endif /* _TMP102_H */
