#ifndef _MQUEUE_WRAPPER_H
#define _MQUEUE_WRAPPER_H

#include "FreeRTOS.h"
#include "queue.h"

struct mq_attr {
  int32_t mq_msgsize;
  int32_t mq_curmsgs;
};

#define mqd_t QueueHandle_t
#define mq_unlink(queue)
#define mq_send(queue, msg, size, priority) xQueueSend(queue, msg, portMAX_DELAY)
#define mq_receive(queue, msg, size, priority) xQueueReceive(queue, msg, portMAX_DELAY)


int32_t mq_getattr(mqd_t queue, struct mq_attr * attr);
mqd_t mq_open(char * name, uint32_t mode, uint32_t perm, void * others);
int32_t mq_close(mqd_t queue);
#endif // _MQUEUE_WRAPPER_H
