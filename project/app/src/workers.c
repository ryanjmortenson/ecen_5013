/** @file workers.c
*
* @brief Main Task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "linkedlist.h"
#include "log.h"
#include "project_defs.h"
#include "workers.h"
#include "profiler.h"

extern int abort_signal;

#define WORKER_QUEUE "/worker"

// Worker threads
static uint32_t num_workers;
pthread_t * workers;

// Registration
typedef struct registration {
  CALLBACK cb;
  type_t  type;
} registartion_t;

// Registration linked list and protection mutex
static node_t * reg_head;
pthread_rwlock_t ll_rwlock = PTHREAD_RWLOCK_INITIALIZER;

/*
 * \brief get_readable_queue: Create a readable queue
 *
 * \return: queue descriptor
 *
 */
static mqd_t get_readable_queue()
{
  return mq_open(WORKER_QUEUE, O_RDONLY | O_CREAT, S_IRWXU, NULL);
}

/*
 * \brief worker_thread: Worker function for worker threads
 *
 * \param param: NULL
 * \return: NULL
 *
 */
void * worker_thread(void * param)
{
  mqd_t msg_q;
  struct mq_attr attr;
  status_t status = SUCCESS;
  int32_t res = 0;
  message_t msg;
  registartion_t * reg;
  node_t * iter;

  do
  {
    msg_q = get_readable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not open queue, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    res = mq_getattr(msg_q, &attr);
    if (res < 0)
    {
      LOG_ERROR("Could not read queue attributes, %s", strerror(errno));
      status = FAILURE;
      break;
    }
  } while(0);

  if (status == SUCCESS)
  {
    while (!abort_signal)
    {
      res = mq_receive(msg_q, (char *)&msg, attr.mq_msgsize, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not receive, %s", strerror(errno));
      }

      if (msg.type == SHUTDOWN)
      {
        break;
      }

      // Read lock the linked list
      pthread_rwlock_rdlock(&ll_rwlock);
      res = ll_iter(reg_head, &iter);
      while (res == LL_ENUM_NO_ERROR)
      {
        res = ll_iter_next(&iter, (void *)&reg);
        if (res == LL_ENUM_NO_ERROR && (reg->type & msg.type))
        {
          reg->cb((void *)msg.msg);
        }
      }
      pthread_rwlock_unlock(&ll_rwlock);
    }
  }

  // Shutdown everything
  mq_close(msg_q);
  pthread_exit((void *)&status);
}

/*!
* @brief Compare registrations
* @return SUCCESS/FAILURE
*/
static uint8_t compare(void * data1, void * data2)
{
  registartion_t * reg1 = (registartion_t *) data1;
  registartion_t * reg2 = (registartion_t *) data2;

  if ((reg1->type == reg2->type) && (reg1->cb == reg2->cb))
  {
    return 1;
  }
  return 0;
}

mqd_t get_writeable_queue()
{
  return mq_open(WORKER_QUEUE, O_NONBLOCK | O_WRONLY | O_CREAT, S_IRWXU, NULL);
}

status_t send_msg(mqd_t msg_q, type_t type, void * data, uint32_t len)
{
  FUNC_ENTRY;
  CHECK_NULL(data);
  message_t msg;
  status_t status = SUCCESS;

  if (msg_q > 0)
  {
    msg.type = type;
    memcpy(&msg.msg, data, len);
    if (mq_send(msg_q, (char *)&msg, sizeof(msg), 0) < 0)
    {
      LOG_ERROR("Could not send message %s", strerror(errno));
      status = FAILURE;
    }
  }
  else
  {
    status = FAILURE;
  }
  return status;
}

status_t register_cb(type_t type, CALLBACK cb)
{
  registartion_t * reg;
  status_t res = FAILURE;

  reg = malloc(sizeof(reg));

  if (reg != NULL)
  {
    // Set registration struct
    reg->type = type;
    reg->cb = cb;

    // Write lock the linked list
    pthread_rwlock_wrlock(&ll_rwlock);
    if (ll_insert(reg_head, reg, INSERT_AT_END) == LL_ENUM_NO_ERROR)
    {
      res = SUCCESS;
    }
    pthread_rwlock_unlock(&ll_rwlock);
  }
  return res;
}

status_t unregister_cb(type_t type, CALLBACK cb)
{
  registartion_t reg;
  registartion_t * preg = &reg;
  status_t res = FAILURE;
  int32_t index = 0;

  // Set registration struct
  reg.type = type;
  reg.cb = cb;

  // Write lock the linked list
  pthread_rwlock_wrlock(&ll_rwlock);
  if (ll_search(reg_head, &reg, compare, &index) == LL_ENUM_NO_ERROR)
  {
    if (ll_remove(reg_head, (void *)&preg, index) == LL_ENUM_NO_ERROR)
    {
      LOG_MED("Removing %p", preg);
      free(preg);
      res = SUCCESS;
    }
  }
  pthread_rwlock_unlock(&ll_rwlock);
  return res;
}

status_t init_workers(uint32_t num)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  num_workers = num;
  int32_t res;

  // Clean up old queue in case there is junk left in it
  mq_unlink(WORKER_QUEUE);

  do
  {
    // Create a linked list for registrations
    if (ll_init(&reg_head) != LL_ENUM_NO_ERROR)
    {
      LOG_ERROR("Could not create registration linked list");
      status = FAILURE;
    }

    workers = malloc(num_workers*sizeof(*workers));
    if (workers == NULL)
    {
      LOG_ERROR("Could not malloc workers");
      status = FAILURE;
    }
  } while(0);

  if (status == SUCCESS)
  {
    // Create worker threads
    for (uint32_t i = 0; i < num_workers; i++)
    {
      LOG_LOW("Creating worker thread %d", i);
      res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not set cancel type, %s", strerror(res));
        status = FAILURE;
        break;
      }

      res = pthread_create(&workers[i], NULL, worker_thread, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could create pthread, %s", strerror(res));
        status = FAILURE;
        break;
      }
    }
  }
  return status;
}

status_t dest_workers()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;

  // Create worker threads
  for (uint32_t i = 0; i < num_workers; i++)
  {
    pthread_cancel(workers[i]);
    LOG_LOW("Worker thread %d cancelled", i);
  }

  // Create worker threads
  for (uint32_t i = 0; i < num_workers; i++)
  {
    pthread_join(workers[i], NULL);
    LOG_LOW("Worker thread joined");
  }

  // Create a linked list for registrations
  if (ll_destroy(reg_head) != LL_ENUM_NO_ERROR)
  {
    LOG_ERROR("Couldn't destroy linked list");
    status = FAILURE;
  }

  // Clean up old queue in case there is junk left in it
  mq_unlink(WORKER_QUEUE);
  return status;
}

status_t flush_queue()
{
  FUNC_ENTRY;
  struct mq_attr attr;
  message_t msg;

  // Get a readable queue and the attributes
  mqd_t msg_q = get_readable_queue();
  mq_getattr(msg_q, &attr);

  // Loop over messages in queue until empty
  while (attr.mq_curmsgs)
  {
    mq_receive(msg_q, (char *)&msg, attr.mq_msgsize, NULL);
    mq_getattr(msg_q, &attr);
  }
  return SUCCESS;
}
