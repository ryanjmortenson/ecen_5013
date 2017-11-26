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

status_t init_air()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  int32_t res = 0;

  do
  {
    res = ccs811_init(I2C_BUS);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not init apds9301");
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

    if (send_hb_setup(PERIOD_US / 1000000, AIR_TASK))
    {
      LOG_ERROR("Could not set up heartbeat");
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
    SEND_INIT_COMPLETE();
  } while(0);
  return status;
}

status_t dest_air()
{
  FUNC_ENTRY;
  int32_t res = 0;
  status_t status = SUCCESS;

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

  mq_close(msg_q);
  return status;
}
