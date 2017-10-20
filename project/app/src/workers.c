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
#include <unistd.h>

#include "linkedlist.h"
#include "log.h"
#include "project_defs.h"
#include "workers.h"

// Worker threads
#define NUM_WORKERS (5)
pthread_t workers[NUM_WORKERS];

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
  int32_t msg = 0;
  int32_t reg_len;
  registartion_t * reg;

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
      LOG_ERROR("Could not open queue, %s", strerror(errno));
      status = FAILURE;
      break;
    }
  } while(0);

  if (status == SUCCESS)
  {
    while (1)
    {
      res = mq_receive(msg_q, (char *)&msg, attr.mq_msgsize, NULL);
      LOG_HIGH("pid: %d msg: %d", pthread_self(), msg);
      if (res < 0)
      {
        LOG_ERROR("Couldn't not receive, %s", strerror(errno));
      }

      // Read lock the linked list
      pthread_rwlock_rdlock(&ll_rwlock);
      res = ll_size(reg_head, &reg_len);
      for (int i = 0; i < reg_len; i++)
      {
        res = ll_peek(reg_head, (void *)&reg, i);
        if (res == LL_ENUM_NO_ERROR && (reg->type & msg))
        {
          reg->cb((void *)&msg);
        }
      }
      pthread_rwlock_unlock(&ll_rwlock);
    }
  }

  // Shutdown everything
  mq_close(msg_q);
  mq_unlink(WORKER_QUEUE);
  pthread_exit((void *)&status);
}

mqd_t get_writeable_queue()
{
  return mq_open(WORKER_QUEUE, O_WRONLY | O_CREAT, S_IRWXU, NULL);
}

status_t register_cb(type_t type, CALLBACK cb)
{
  registartion_t * reg;
  status_t res = FAILURE;

  reg = malloc(sizeof(reg));

  if (reg != NULL)
  {
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

status_t init_workers()
{
  status_t status = SUCCESS;

  if (ll_init(&reg_head) != LL_ENUM_NO_ERROR)
  {
    status = FAILURE;
  }

  for (uint32_t i = 0; i < NUM_WORKERS; i++)
  {
    LOG_FATAL("Creating thread");
    pthread_create(&workers[i], NULL, worker_thread, NULL);
  }
  return status;
}
