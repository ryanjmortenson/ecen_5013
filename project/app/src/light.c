/** @file light.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <mqueue.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "apds9301.h"
#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "workers.h"

#define PERIOD_US (1000000)

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

  SEND_LOG_HIGH("Lux %f", stale_reading);

  if (send_msg(msg_q, &out, &light_rsp, sizeof(light_rsp)) != SUCCESS)
  {
    LOG_ERROR("Could not send light response");
  }
  return NULL;
}

/*!
* @brief Main light task
* @param param NULL
* @return NULL
*/
void * light_thread(void * param)
{
  FUNC_ENTRY;
  float cur_reading;

  while(!abort_signal)
  {
    send_hb(LIGHT_TASK);
    if (apds9301_r_lux(&cur_reading) != SUCCESS)
    {
      LOG_ERROR("Could not read lux for stashed reading");
    }

    if (stale_reading - cur_reading > 1.0f)
    {
      SEND_LOG_HIGH("The light lux level decreased atleast 1 lux in 1 reading");
    }
    else if (stale_reading - cur_reading < -1.0f)
    {
      SEND_LOG_HIGH("The light lux level increased atleast 1 lux in 1 reading");
    }
    stale_reading = cur_reading;
    usleep(PERIOD_US);
  }
  return NULL;
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

  // Emperically derived "darkness"
  if (lux < .1f)
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

  light_req.staleness = staleness;
  if (send_msg(msg_q, &msg, &light_req, sizeof(light_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send light request");
    status = FAILURE;
  }
  return status;
}

status_t init_light()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  int32_t res = 0;

  do
  {
    res = apds9301_init(2);
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

    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }

    if (send_hb_setup(PERIOD_US / 1000000, LIGHT_TASK))
    {
      LOG_ERROR("Could not set up heartbeat");
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
    SEND_INIT_COMPLETE();
  } while(0);
  return status;
}

status_t dest_light()
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

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
    LOG_ERROR("Temp task could not join, continuing with shutdown %s",
              strerror(res));
    status = FAILURE;
  }
  else
  {
    LOG_HIGH("Light task joined");
  }

  // Unregister light request handler
  res = unregister_cb(LIGHT_REQ, LIGHT_TASK, light_req);
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

  mq_close(msg_q);
  return status;
}
