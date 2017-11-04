/** @file led.c
*
* @brief Control for user led
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "led.h"
#include "log.h"
#include "project_defs.h"

#define LED_PATH "/sys/class/leds/beaglebone:green:usr%d/%s"
#define LED_PATH_TRIGGER "trigger"
#define LED_PATH_BRIGHTNESSS "brightness"
#define NONE "none"
#define MAX_PATH_LEN (128)
#define MAX_BRIGHTNESS_LEN (8)

status_t set_brightness(uint8_t led_num, uint8_t brightness)
{
  int32_t bfd;
  int32_t res;
  status_t status = SUCCESS;
  char file_name[MAX_PATH_LEN];
  char bright_str[MAX_BRIGHTNESS_LEN];

  do
  {
    res = snprintf(file_name, MAX_PATH_LEN, LED_PATH, led_num, LED_PATH_BRIGHTNESSS);
    if (res < 0)
    {
      LOG_ERROR("Could not create LED brightness file name");
      status = FAILURE;
      break;
    }

    bfd = open(file_name, O_WRONLY);
    if (bfd < 0)
    {
      LOG_ERROR("Could not open LED brightness file: %s", file_name);
      status = FAILURE;
      break;
    }

    res = snprintf(bright_str, MAX_BRIGHTNESS_LEN, "%d", brightness);
    if (res < 0)
    {
      LOG_ERROR("Could not create LED brightness string");
      status = FAILURE;
      break;
    }

    res = write(bfd, bright_str, res);
    if (res < 0)
    {
      LOG_ERROR("Could not write to LED brightness file");
      status = FAILURE;
      break;
    }

    res = close(bfd);
    if (res < 0)
    {
      LOG_ERROR("Could not close LED brightness file");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

static inline status_t set_trigger(uint8_t led_num)
{
  int32_t tfd;
  int32_t res;
  status_t status = SUCCESS;
  char file_name[MAX_PATH_LEN];

  do
  {
    res = snprintf(file_name, MAX_PATH_LEN, LED_PATH, led_num, LED_PATH_TRIGGER);
    if (res < 0)
    {
      LOG_ERROR("Could not create LED trigger file name");
      status = FAILURE;
      break;
    }

    tfd = open(file_name, O_WRONLY);
    if (tfd < 0)
    {
      LOG_ERROR("Could not LED trigger file: %s", file_name);
      status = FAILURE;
      break;
    }

    res = write(tfd, NONE, strlen(NONE));
    if (res < 0)
    {
      LOG_ERROR("Could not write %s to LED trigger file", NONE);
      status = FAILURE;
      break;
    }

    res = close(tfd);
    if (res < 0)
    {
      LOG_ERROR("Could not close LED trigger file");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t init_led(uint8_t led_num)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  do
  {
    if (set_trigger(led_num) != SUCCESS)
    {
      LOG_ERROR("Could not set LED trigger");
      status = FAILURE;
      break;
    }

    if (set_brightness(led_num, OFF_BRIGHTNESS) != SUCCESS)
    {
      LOG_ERROR("Could not set LED brightness");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}
