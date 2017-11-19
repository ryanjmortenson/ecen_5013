/** @file led_module.h
*
* @brief Header for sharing types between module and user space
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef __LED_MODULE_H__
#define __LED_MODULE_H__

typedef enum read_cmd {
  READ_ALL,
  READ_PERIOD,
  READ_DUTY_CYCLE,
  READ_LED_STATE,
  READ_BLINK_STATE
} read_cmd_t;

#endif // __LED_MODULE_H__
