/** @file led.h
*
* @brief BBB Led control
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _LED_H
#define _LED_H

#include "project_defs.h"

#define ON_BRIGHTNESS (255)
#define OFF_BRIGHTNESS (0)

/*!
* @brief Initialize BBB user led
* @param[in] led_name usr led number
* @return status of setting up led
*/
status_t init_led(uint8_t led_num);

/*!
* @brief Set the brightness of BBB led
* @param[in] led_name usr led number
* @param[in] brightness of led
* @return status of setting led brightness
*/
status_t set_brightness(uint8_t led_num, uint8_t brightness);

#endif /* _LED_H */