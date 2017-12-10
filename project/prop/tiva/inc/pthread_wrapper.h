/** @file mqueue_wrapper.h
*
* @brief Wrappers to use POSIX calls with FreeRTOS
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _PTHREAD_WRAPPER_H
#define _PTHREAD_WRAPPER_H

#include "semphr.h"
#include "project_defs.h"

status_t mutex_init(SemaphoreHandle_t * sem);

// Defines for pthread
#define pthread_t TaskHandle_t
#define pthread_create(thread, attrs, func, params) xTaskCreate(func, "THREAD", 1024, NULL, 9, thread);
#define PTHREAD_RETURN_TYPE void
#define PTHREAD_RETURN(val) vTaskDelete(NULL)
#define pthread_join(thread, val) (0)
#define pthread_cancel(thread) 0; vTaskDelete(thread)
#define pthread_setcanceltype(type, val) (0)
#define PTHREAD_CANCEL_ASYNCHRONOUS (0)

// Defines for rwlock unfortunately free rtos does not have a rwlock so use semaphore
#define pthread_rwlock_t SemaphoreHandle_t
#define pthread_rwlock_rdlock(lock) xSemaphoreTake(*lock, portMAX_DELAY)
#define pthread_rwlock_wrlock(lock) xSemaphoreTake(*lock, portMAX_DELAY)
#define pthread_rwlock_unlock(lock) xSemaphoreGive(*lock)
#define pthread_rwlock_init(lock, attr) mutex_init(lock)

// Defines for mutex
#define pthread_mutex_t SemaphoreHandle_t
#define pthread_mutex_lock(lock) xSemaphoreTake(*lock, portMAX_DELAY)
#define pthread_mutex_unlock(lock) xSemaphoreGive(*lock)
#define pthread_mutex_init(lock, attr) mutex_init(lock)
#define pthread_mutex_destroy(lock) vSemaphoreDelete(lock)

// Define for usleep
#define usleep(us) vTaskDelay((us)/ (portTICK_PERIOD_MS * 1000))

#endif // _PTHREAD_WRAPPER_H
