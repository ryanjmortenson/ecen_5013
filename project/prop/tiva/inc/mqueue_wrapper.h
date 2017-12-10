/** @file mqueue_wrapper.h
*
* @brief Wrappers to use POSIX calls with FreeRTOS
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _MQUEUE_WRAPPER_H
#define _MQUEUE_WRAPPER_H

#include "FreeRTOS.h"
#include "queue.h"

// Faked out mq attr struct
struct mq_attr {
  int32_t mq_msgsize;
  int32_t mq_curmsgs;
};

// Define POSIX functions as FreeRTOS functions
#define mqd_t QueueHandle_t
#define mq_unlink(queue)
#define mq_send(queue, msg, size, priority) xQueueSend(queue, msg, portMAX_DELAY)
#define mq_receive(queue, msg, size, priority) xQueueReceive(queue, msg, portMAX_DELAY)

/*!
* @brief Get attrs from structure
* @param queue mq file descriptor
* @param attr mq attrs
* @return status of getting attr
*/
int32_t mq_getattr(mqd_t queue, struct mq_attr * attr);

/*!
* @brief Open a queue (FreeRTOS)
* @param name (not used)
* @param mode (not used)
* @param perm (not used)
* @param others (not used)
* @return status opening queue
*/
mqd_t mq_open(char * name, uint32_t mode, uint32_t perm, void * others);

/*!
* @brief Close a queue
* @param queue mq descriptor
* @return status closing queue
*/
int32_t mq_close(mqd_t queue);
#endif // _MQUEUE_WRAPPER_H
