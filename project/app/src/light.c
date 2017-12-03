/** @file light.c
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

#include "apds9301.h"
#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "workers.h"

#define PERIOD_US (1000000)
#define DARK (50.0f)
#define I2C_BUS (2)

// Map staleness enum to staleness string
extern char * staleness_str[];

// Abort from main thread
extern int32_t abort_signal;
static const task_id_t TASK_ID = LIGHT_TASK;

static mqd_t msg_q;
static pthread_t light_task;
static float stale_reading;

/*!
* @brief Handle a light request
* @param param msg holding light request
* @return NULL
*/
void * light_req(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  light_req_t * light_req = (light_req_t *)in->msg;
  light_rsp_t light_rsp;
  message_t out = MSG_INIT(LIGHT_RSP, in->from, LIGHT_TASK);

  // Determine what stalness was requested and send the response
  if (light_req->staleness == STALENESS_NEW)
  {
    SEND_LOG_MED("Reading new lux");
    apds9301_r_lux(&light_rsp.lux);
  }
  else
  {
    SEND_LOG_MED("Getting stale lux");
    light_rsp.lux = stale_reading;
  }

  if (send_msg(msg_q, &out, &light_rsp, sizeof(light_rsp)) != SUCCESS)
  {
    LOG_ERROR("Could not send light response");
  }
  return NULL;
}

/*!
* @brief Handle is dark requests
* @param param msg holding is dark request
* @return NULL
*/
void * is_dark_req(void * param)
{
  FUNC_ENTRY;
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  is_dark_rsp_t dark_rsp;
  message_t out = MSG_INIT(IS_DARK_RSP, in->from, LIGHT_TASK);

  do
  {
    if (is_dark(&dark_rsp.dark) != SUCCESS)
    {
      LOG_ERROR("Could not get darkness");
      break;
    }

    if (send_msg(msg_q, &out, &dark_rsp, sizeof(dark_rsp)) != SUCCESS)
    {
      LOG_ERROR("Could not send light response");
      break;
    }
  } while(0);
  return NULL;
}

/*!
* @brief Main light task
* @param param NULL
* @return NULL
*/
PTHREAD_RETURN_TYPE light_thread(void * param)
{
  FUNC_ENTRY;
  float cur_reading;

  while(!abort_signal)
  {
    // Periodically send heartbeat, read lux, and determine if it an sudden
    // change occured
    send_hb(LIGHT_TASK);
    if (apds9301_r_lux(&cur_reading) != SUCCESS)
    {
      LOG_ERROR("Could not read lux for stashed reading");
    }

    // Light and dark reading were retrieved from wikipedia
    if (stale_reading >= DARK && cur_reading < DARK)
    {
      SEND_LOG_FATAL("Seems to have changed from light to dark");
    }
    else if (stale_reading <= DARK && cur_reading > DARK)
    {
      SEND_LOG_FATAL("Seems to have changed from dark to light");
    }
    // Set the stale reading
    stale_reading = cur_reading;
    usleep(PERIOD_US);
  }
  PTHREAD_RETURN(NULL);
}

status_t is_dark(uint8_t * dark)
{
  FUNC_ENTRY;
  CHECK_NULL(dark);
  float lux;
  status_t status = SUCCESS;
  *dark = 0;

  if (apds9301_r_lux(&lux) != SUCCESS)
  {
    LOG_ERROR("Could not read lux for is dark");
    status = FAILURE;
  }

  if (lux < DARK)
  {
    *dark = 1;
  }
  return status;
}

status_t send_light_req(staleness_t staleness, task_id_t from)
{
  FUNC_ENTRY;
  light_req_t light_req;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(LIGHT_REQ, LIGHT_TASK, from);

  // Fill out light request and send
  light_req.staleness = staleness;
  if (send_msg(msg_q, &msg, &light_req, sizeof(light_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send light request");
    status = FAILURE;
  }
  return status;
}

status_t send_is_dark_req(task_id_t from)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  message_t msg = MSG_INIT(IS_DARK_REQ, LIGHT_TASK, from);

  // Fill out light request and send
  if (send_msg(msg_q, &msg, NULL, 0) != SUCCESS)
  {
    LOG_ERROR("Could not send is dark request");
    status = FAILURE;
  }
  return status;
}

status_t init_light(uint8_t start_task)
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
      if (send_hb_setup(PERIOD_US / 1000000, LIGHT_TASK))
      {
        LOG_ERROR("Could not set up heartbeat");
        status = FAILURE;
        break;
      }

      res = apds9301_init(I2C_BUS);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not init apds9301");
        status = FAILURE;
        break;
      }

      // Register light request handler
      res = register_cb(LIGHT_REQ, LIGHT_TASK, light_req);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not register callback, %s", strerror(errno));
        status = FAILURE;
        break;
      }

      res = register_cb(IS_DARK_REQ, LIGHT_TASK, is_dark_req);
      if (res == FAILURE)
      {
        LOG_ERROR("Could not register callback, %s", strerror(errno));
        status = FAILURE;
        break;
      }

      res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not set cancellability of light task, %s",
                  strerror(res));
        status = FAILURE;
        break;
      }

      res = pthread_create(&light_task, NULL, light_thread, NULL);
      if (res < 0)
      {
        LOG_ERROR("Could not create light task, %s", strerror(res));
        status = FAILURE;
        break;
      }
    }
    SEND_INIT_COMPLETE();
  } while(0);
  return status;
}

status_t dest_light(uint8_t dest_task)
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

  if (dest_task == 1)
  {
    res = pthread_cancel(light_task);
    if (res < 0)
    {
      LOG_ERROR("Could not create light task, continuing with shutdown, %s",
                strerror(res));
      status = FAILURE;
    }

    res = pthread_join(light_task, NULL);
    if (res < 0)
    {
      LOG_ERROR("Light task could not join, continuing with shutdown %s",
                strerror(res));
      status = FAILURE;
    }
    else
    {
      LOG_HIGH("Light task joined");
    }

    // Unregister light request handlers
    res = unregister_cb(LIGHT_REQ, LIGHT_TASK, light_req);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not unregister callback");
      status = FAILURE;
    }

    res = unregister_cb(IS_DARK_REQ, LIGHT_TASK, is_dark_req);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not unregister callback");
      status = FAILURE;
    }

    res = apds9301_dest();
    if (res == FAILURE)
    {
      LOG_ERROR("Could not destroy apds9301");
      status = FAILURE;
    }
  }
  mq_close(msg_q);
  return status;
}
