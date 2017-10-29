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
#include "tmp102.h"
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
static uint32_t stale_reading;

status_t send_temp_req(temp_units_t temp_units, staleness_t staleness)
{
  FUNC_ENTRY;
  temp_req_t temp_req;
  status_t status = SUCCESS;

  temp_req.temp_units = temp_units;
  temp_req.staleness = staleness;
  if (send_msg(msg_q, TEMP_REQ, &temp_req, sizeof(temp_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send temp reading");
    status = FAILURE;
  }
  return status;
}

void * handle_temp_req(void * param)
{
  FUNC_ENTRY;
  temp_req_t * temp_req = (temp_req_t *)param;
  temp_rsp_t temp_rsp;
  uint8_t temp;
  SEND_LOG_FATAL("Temp req for %s %s",
                 temp_units_str[temp_req->temp_units],
                 staleness_str[temp_req->staleness]);

  temp_rsp.temp_units = temp_req->temp_units;
  if (temp_req->staleness == STALENESS_NEW)
  {
    SEND_LOG_HIGH("Reading new temp");
    tmp102_r_tmp(&temp);
    temp_rsp.temp = (uint32_t)temp;
  }
  else
  {
    SEND_LOG_HIGH("Getting stale temp");
    temp_rsp.temp = stale_reading;
  }

  if (send_msg(msg_q, TEMP_RSP, &temp_rsp, sizeof(temp_rsp)) != SUCCESS)
  {
    LOG_ERROR("Could not send temp reading");
  }
  return NULL;
}

void * temp_thread(void * param)
{
  FUNC_ENTRY;

  while(!abort_signal)
  {
    usleep(PERIOD_US);
    if (tmp102_r_tmp((uint8_t *) &stale_reading) != SUCCESS)
    {
      LOG_ERROR("Could not read temp for stashed reading");
    }
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
      LOG_ERROR("Could not set cancellability of temp task, %s"),
                strerror(res);
      status = FAILURE;
      break;
    }

    res = pthread_create(&temp_task, NULL, temp_thread, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not create temp task, %s", strerror(res));
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t dest_temp()
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

  res = pthread_cancel(temp_task);
  if (res < 0)
  {
    LOG_ERROR("Could not create temp task, continuing with shutdown, %s",
              strerror(res));
    status = FAILURE;
  }

  res = pthread_join(temp_task, NULL);
  if (res < 0)
  {
    LOG_ERROR("Temp task could not join, continuing with shutdown %s",
              strerror(res));
    status = FAILURE;
  }
  else
  {
    LOG_HIGH("Temp task joined");
  }

  // Unregister temp request handler
  res = unregister_cb(TEMP_REQ, handle_temp_req);
  if (res == FAILURE)
  {
    LOG_ERROR("Could not unregister callback, %s", strerror(errno));
    status = FAILURE;
  }

  mq_close(msg_q);
  return status;
}
