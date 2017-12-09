/** @file main_task.c
*
* @brief Main project tasks
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "air.h"
#include "light.h"
#include "humidity.h"
#include "log.h"
#include "log_msg.h"
#include "main_client_task.h"
#include "main_helper.h"
#include "project_defs.h"
#include "client_task.h"
#include "temp.h"
#include "workers.h"

#ifndef TIVA
#include <mqueue.h>
#include <pthread.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#include "pthread_wrapper.h"
#include "mqueue_wrapper.h"
#endif // TIVA

// Set task id for SEND_LOG* messages
static const task_id_t TASK_ID = MAIN_TASK;

int abort_signal;

// Writeable message queue
mqd_t msg_q;

void main_client_task()
{
  while(!abort_signal)
  {
    LOG_FATAL("Still Awake");
    usleep(1000000);
  }
}

status_t init_main_client_task(int argc, char *argv[])
{
  FUNC_ENTRY;
  char * file_name;
  uint32_t num_workers = 1;
  status_t status = SUCCESS;

#ifndef TIVA
  // Initialize log to print errors
  log_init();
#endif // TIVA

  // The first argument is required which is the log name
  if (argc < 2)
  {
    LOG_ERROR("The log name is a required argument");
    return FAILURE;
  }
  else
  {
    file_name = argv[1];
    LOG_HIGH("Log output file is %s", file_name);
  }

  // Second argument is number of worker threads
  if (argc > 2)
  {
    num_workers = atoi(argv[2]);
    if (num_workers < 1 || num_workers > 20)
    {
      LOG_ERROR("Number of worker threads must be between 1 and 20");
      return FAILURE;
    }
  }

  // Initialize the rest
  do
  {
    if (init_workers(num_workers) != SUCCESS)
    {
      LOG_ERROR("Could not initialize workers");
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

    if (client_init() != SUCCESS)
    {
      LOG_ERROR("Could not initialize client");
      status = FAILURE;
      break;
    }

    if (log_msg_init(file_name, 1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_temp(1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize temp");
      status = FAILURE;
      break;
    }

    if (init_air(1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize air");
      status = FAILURE;
      break;
    }

    if (init_humidity(1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize humidity");
      status = FAILURE;
      break;
    }

    if (init_light(1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize light");
      status = FAILURE;
      break;
    }

#ifdef BBB
    if (init_led(USR3_LED) != SUCCESS)
    {
      LOG_ERROR("Could not initialize led");
      status = FAILURE;
      break;
    }
#endif // BBB
    SEND_INIT_COMPLETE();
  } while(0);

#ifdef BBB
  if (status == FAILURE)
  {
    set_brightness(USR3_LED, ON_BRIGHTNESS);
  }
#endif // BBB
  return status;
}

status_t dest_main_client_task()
{
  status_t status = SUCCESS;

  if (dest_humidity(1) != SUCCESS)
  {
    LOG_ERROR("Could not destroy humidity task");
    status = FAILURE;
  }

  if (dest_temp(1) != SUCCESS)
  {
    LOG_ERROR("Could not destroy temp task");
    status = FAILURE;
  }

  if (dest_air(1) != SUCCESS)
  {
    LOG_ERROR("Could not destroy air task");
    status = FAILURE;
  }

  if (dest_light(1) != SUCCESS)
  {
    LOG_ERROR("Could not destroy light task");
    status = FAILURE;
  }

  if (dest_workers() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }

#ifdef BBB
  if (status == FAILURE)
  {
    set_brightness(USR3_LED, ON_BRIGHTNESS);
  }
#endif // BBB

#ifndef TIVA
  log_destroy();
#endif // TIVA
  return status;
}
