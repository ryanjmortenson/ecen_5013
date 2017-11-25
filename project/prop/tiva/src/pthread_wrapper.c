#include "FreeRTOS.h"
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

