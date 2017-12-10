/** @file pthread_wrapper.c
*
* @brief Wrappers to use POSIX calls with FreeRTOS
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <sys/time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "project_defs.h"
#include "pthread_wrapper.h"

status_t mutex_init(SemaphoreHandle_t * sem)
{
  status_t status = SUCCESS;

  *sem = xSemaphoreCreateBinary();
  if (*sem == NULL)
  {
    status = FAILURE;
  }
  else
  {
    xSemaphoreGive(*sem);
  }
  return status;
}

/*!
* @brief Used to get up tick count of tiva
* @param tv timeval struct of fill out
* @param tz time zone (not used)
* @return status
*/
status_t _gettimeofday(struct timeval * tv, struct timezone * tz)
{
  int ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
  tv->tv_sec = ms / 1000;
  tv->tv_usec = (ms * 1000) ;
  return 0;
}
