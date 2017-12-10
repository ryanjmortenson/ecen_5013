/** @file humidity.c
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

#include "humidity.h"
#include "bme280.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "workers.h"

#define PERIOD_US (1000000)
#define I2C_BUS (2)

// Abort from main thread
extern int32_t abort_signal;
static const task_id_t TASK_ID = HUMIDITY_TASK;

static mqd_t msg_q;
static pthread_t humidity_task = 0;
static uint16_t stale_reading;

void * humidity_req(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  message_t out = MSG_INIT(HUMIDITY_RSP, in->from, HUMIDITY_TASK);
  humidity_req_t * req = (humidity_req_t *)in->msg;
  humidity_rsp_t humidity_rsp;

  if (req->staleness == STALENESS_NEW)
  {
    bme280_r_humidity(&humidity_rsp.humidity);
  }
  else
  {
    humidity_rsp.humidity = stale_reading;
  }

  if (send_msg(msg_q, &out, &humidity_rsp, sizeof(humidity_rsp)) != SUCCESS)
  {
    LOG_ERROR("Could not send humidity reading");
  }
  return NULL;
}

status_t send_humidity_req(staleness_t staleness, task_id_t from)
{
  FUNC_ENTRY;
  humidity_req_t humidity_req;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(HUMIDITY_REQ, HUMIDITY_TASK, from);

  // Fill out temp request and send
  humidity_req.staleness = staleness;
  if (send_msg(msg_q, &msg, &humidity_req, sizeof(humidity_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send humidity request");
    status = FAILURE;
  }
  return status;
}

/*!
* @brief Main humidity task
* @param param NULL
* @return NULL
*/
PTHREAD_RETURN_TYPE humidity_thread(void * param)
{
  FUNC_ENTRY;

  while(!abort_signal)
  {
    // Periodically send heartbeat
    send_hb(HUMIDITY_TASK);
    bme280_r_humidity(&stale_reading);
    usleep(PERIOD_US);
  }
  PTHREAD_RETURN(NULL);
}

status_t init_humidity(uint8_t start_task)
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
      if (send_hb_setup(PERIOD_US / 1000000, HUMIDITY_TASK))
      {
        LOG_ERROR("Could not set up heartbeat");
        status = FAILURE;
        break;
      }

      res = bme280_init(I2C_BUS);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not init ccs811");
        status = FAILURE;
        break;
      }

      // Register humidity request handler
      res = register_cb(HUMIDITY_REQ, HUMIDITY_TASK, humidity_req);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not register callback, %s", strerror(errno));
        status = FAILURE;
        break;
      }
      res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not set cancellability of humidity task, %s",
                  strerror(res));
        status = FAILURE;
        break;
      }

      res = pthread_create(&humidity_task, NULL, humidity_thread, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not create humidity task, %s", strerror(res));
        status = FAILURE;
        break;
      }
    }
    SEND_INIT_COMPLETE();
  } while(0);
  return status;
}

status_t dest_humidity(uint8_t dest_task)
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

  if (dest_task == 1)
  {
    res = pthread_cancel(humidity_task);
    if (res != 0)
    {
      LOG_ERROR("Could not create humidity task, continuing with shutdown, %s",
                strerror(res));
      status = FAILURE;
    }

    res = pthread_join(humidity_task, NULL);
    if (res < 0)
    {
      LOG_ERROR("Air task could not join, continuing with shutdown %s",
                strerror(res));
      status = FAILURE;
    }
    else
    {
      LOG_HIGH("Humidity task joined");
    }

    res = bme280_dest();
    if (res == FAILURE)
    {
      LOG_ERROR("Could not destroy apds9301");
      status = FAILURE;
    }
    humidity_task = 0;
  }
  mq_close(msg_q);
  return status;
}
