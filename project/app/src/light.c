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

#include "light.h"
#include "log.h"
#include "project_defs.h"
#include "workers.h"

#define PERIOD_US (1000000)

// Abort from main thread
extern int32_t abort_signal;

static mqd_t msg_q;
static pthread_t light_task;

status_t send_light_req(staleness_t staleness)
{
  FUNC_ENTRY;
  light_req_t light_req;
  status_t status = SUCCESS;

  light_req.staleness = staleness;
  if (send_msg(msg_q, LIGHT_REQ, &light_req, sizeof(light_req)) != SUCCESS)
  {
    LOG_ERROR("Could not send light request");
    status = FAILURE;
  }
  return status;
}

void * handle_light_req(void * param)
{
  LOG_HIGH("Handling light request");
  return NULL;
}

void * light_thread(void * param)
{
  FUNC_ENTRY;

  while(!abort_signal)
  {
    LOG_HIGH("Sleeping for %d", PERIOD_US);
    usleep(PERIOD_US);
  }
  return NULL;
}

status_t init_light()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  int32_t res = 0;

  do
  {
    // Register light request handler
    res = register_cb(LIGHT_REQ, handle_light_req);
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
      LOG_ERROR("Could not set cancellability of light task, %s"),
                strerror(res);
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
    LOG_HIGH("Temp task joined");
  }

  // Unregister light request handler
  res = unregister_cb(LIGHT_REQ, handle_light_req);
  if (res == FAILURE)
  {
    LOG_ERROR("Could not unregister callback");
    status = FAILURE;
  }

  mq_close(msg_q);
  return status;
}
