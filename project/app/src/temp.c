/** @file temp.c
*
* @brief Implemenation of temp tasks
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <stdint.h>
#include <mqueue.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "project_defs.h"

#include "log.h"
#include "log_msg.h"
#include "temp.h"
#include "workers.h"

#define PERIOD_US (1000000)

char * temp_units_str[] = {
  "TEMP_UNITS_C",
  "TEMP_UNITS_K",
  "TEMP_UNITS_F"
};

char * staleness_str[] = {
  "STALENESS_NEW",
  "STALENESS_OLD"
};

// Abort from main thread
extern int32_t abort_signal;

static mqd_t msg_q;
static pthread_t temp_task;
static int32_t stashed_reading;

status_t send_temp_req(temp_units_t temp_units, staleness_t staleness)
{
  message_t msg;
  temp_req_t temp_req;
  status_t status = SUCCESS;

  if (msg_q > 0)
  {
    temp_req.temp_units = temp_units;
    temp_req.staleness = staleness;
    msg.type = TEMP_REQ;
    memcpy(&msg.msg, &temp_req, sizeof(temp_req));
    mq_send(msg_q, (char *)&msg, sizeof(msg), 0);
  }
  else
  {
    status = FAILURE;
  }
  return status;
}

void * handle_temp_req(void * param)
{
  temp_req_t * temp_req = (temp_req_t *)param;
  SEND_LOG_FATAL("Temp req for %s %s",
                 temp_units_str[temp_req->temp_units],
                 staleness_str[temp_req->staleness]);
  return NULL;
}

void * temp_thread(void * param)
{
  FUNC_ENTRY;

  while(!abort_signal)
  {
    LOG_HIGH("Sleeping for %d us", PERIOD_US);
    usleep(PERIOD_US);
    stashed_reading++;
  }
  return NULL;
}

status_t init_temp()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  int32_t res = 0;

  do
  {
    // Register temp request handler
    res = register_cb(TEMP_REQ, handle_temp_req);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not register callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }

    res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not set cancellability of temp task");
      status = FAILURE;
      break;
    }

    res = pthread_create(&temp_task, NULL, temp_thread, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not create temp task");
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t dest_temp()
{
  FUNC_ENTRY;
  uint32_t res = 0;
  status_t status = SUCCESS;

  res = pthread_cancel(temp_task);
  if (res < 0)
  {
    LOG_ERROR("Could not create temp task, continuing with shutdown");
    status = FAILURE;
  }

  res = pthread_join(temp_task, NULL);
  if (res < 0)
  {
    LOG_ERROR("Temp task could not join, continuing with shutdown");
    status = FAILURE;
  }
  else
  {
    LOG_HIGH("Temp task joined");
  }

  mq_close(msg_q);
  return status;
}
