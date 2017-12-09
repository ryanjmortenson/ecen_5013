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

status_t _gettimeofday(struct timeval * tv, struct timezone * tz)
{
  int ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
  tv->tv_sec = ms / 1000;
  tv->tv_usec = (ms * 1000) ;
  return 0;
}
