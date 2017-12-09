/** @file air.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#ifndef TIVA
#include <mqueue.h>
#include <pthread.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#include "pthread_wrapper.h"
#include "mqueue_wrapper.h"
#endif // TIVA

#include "air.h"
#include "ccs811.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "workers.h"

#define PERIOD_US (1000000)
#define I2C_BUS (2)

// Abort from main thread
extern int32_t abort_signal;
static const task_id_t TASK_ID = AIR_TASK;

static mqd_t msg_q;
static pthread_t air_task;

void * air_req(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  air_rsp_t air_rsp;
  message_t out = MSG_INIT(AIR_RSP, in->from, AIR_TASK);

  SEND_LOG_MED("Reading air quality");
  if (send_msg(msg_q, &out, &air_rsp, sizeof(air_rsp)) != SUCCESS)
  {
    LOG_ERROR("Could not send air reading");
  }
  return NULL;
}

status_t send_air_req(air_meas_type_t type, staleness_t staleness, task_id_t from)
{
  FUNC_ENTRY;
  air_req_t air_req;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(AIR_REQ, AIR_TASK, from);

  // Fill out temp request and send
  air_req.type = type;
  air_req.staleness = staleness;
  if (send_msg(msg_q, &msg, &air_req, sizeof(air_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send air request");
    status = FAILURE;
  }
  return status;
}

/*!
* @brief Main air task
* @param param NULL
* @return NULL
*/
PTHREAD_RETURN_TYPE air_thread(void * param)
{
  FUNC_ENTRY;

  while(!abort_signal)
  {
    // Periodically send heartbeat
    send_hb(AIR_TASK);
    usleep(PERIOD_US);
  }
  PTHREAD_RETURN(NULL);
}

status_t init_air(uint8_t start_task)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  int32_t res = 0;

  do
  {
    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }

    if (start_task == 1)
    {
      if (send_hb_setup(PERIOD_US / 1000000, AIR_TASK))
      {
        LOG_ERROR("Could not set up heartbeat");
        status = FAILURE;
        break;
      }

      res = ccs811_init(I2C_BUS);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not init ccs811");
        status = FAILURE;
        break;
      }

      // Register temp request handler
      res = register_cb(AIR_REQ, AIR_TASK, air_req);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not register callback, %s", strerror(errno));
        status = FAILURE;
        break;
      }
      res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not set cancellability of air task, %s",
                  strerror(res));
        status = FAILURE;
        break;
      }

      res = pthread_create(&air_task, NULL, air_thread, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not create air task, %s", strerror(res));
        status = FAILURE;
        break;
      }
    }
    SEND_INIT_COMPLETE();
  } while(0);
  return status;
}

status_t dest_air(uint8_t dest_task)
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

  if (dest_task == 1)
  {
    res = pthread_cancel(air_task);
    if (res < 0)
    {
      LOG_ERROR("Could not create air task, continuing with shutdown, %s",
                strerror(res));
      status = FAILURE;
    }

    res = pthread_join(air_task, NULL);
    if (res < 0)
    {
      LOG_ERROR("Air task could not join, continuing with shutdown %s",
                strerror(res));
      status = FAILURE;
    }
    else
    {
      LOG_HIGH("Air task joined");
    }

    res = ccs811_dest();
    if (res == FAILURE)
    {
      LOG_ERROR("Could not destroy apds9301");
      status = FAILURE;
    }
  }
  mq_close(msg_q);
  return status;
}
