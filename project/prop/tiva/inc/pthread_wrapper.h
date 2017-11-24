#ifndef _PTHREAD_WRAPPER_H
#define _PTHREAD_WRAPPER_H

#include "semphr.h"

#define pthread_t TaskHandle_t
#define pthread_create(thread, attrs, func, params) xTaskCreate(func, "THREAD", 1024, NULL, 3, thread);
#define PTHREAD_RETURN_TYPE void
#define PTHREAD_RETURN(val) vTaskDelete(NULL)
#define pthread_join(thread, val) (0)
#define pthread_cancel(thread) 0; vTaskDelete(thread)
#define pthread_setcanceltype(type, val) (0)
#define PTHREAD_CANCEL_ASYNCHRONOUS (0)

#define pthread_rwlock_t SemaphoreHandle_t
#define pthread_rwlock_rdlock(lock) xSemaphoreTake(*lock, portMAX_DELAY)
#define pthread_rwlock_wrlock(lock) xSemaphoreTake(*lock, portMAX_DELAY)
#define pthread_rwlock_unlock(lock) xSemaphoreGive(*lock)
#define PTHREAD_RWLOCK_INITIALIZER xSemaphoreCreateBinary()

#endif // _PTHREAD_WRAPPER_H