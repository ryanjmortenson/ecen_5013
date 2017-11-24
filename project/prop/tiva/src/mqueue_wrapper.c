#include "mqueue_wrapper.h"

static mqd_t queue = NULL;

mqd_t mq_open(char * name, uint32_t mode, uint32_t perm, void * others)
{
  if (queue == NULL)
  {
    queue = xQueueCreate(10, 530);
  }
  return queue;
}

int32_t mq_close(mqd_t queue)
{
  if (queue != NULL)
  {
    vQueueDelete(queue);
    queue = NULL;
  }
  return 0;
}

int32_t mq_getattr(mqd_t queue, struct mq_attr * attr) {
  attr->mq_msgsize = 530;
  attr->mq_curmsgs = uxQueueMessagesWaiting(queue);
  return 0;
}
