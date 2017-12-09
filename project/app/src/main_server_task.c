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
#include "humidity.h"
#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_server_task.h"
#include "main_helper.h"
#include "project_defs.h"
#include "server_task.h"
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

// Writeable message queue
mqd_t msg_q;

status_t init_main_server_task(int argc, char *argv[])
{
  FUNC_ENTRY;
  char * file_name;
  uint32_t num_workers = 1;
  status_t status = SUCCESS;
  int32_t res;
  struct sigaction timer_handler;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize log to print errors
  log_init();

  // Initialzie timers to 0
  init_timers();

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
    // Register signal handler for sigint
    res = sigaction(SIGINT, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT handler %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Register signal handler for sigterm
    res = sigaction(SIGTERM, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT handler %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Register heartbeat signal handler
    timer_handler.sa_sigaction = hb_timeout_handler;
    timer_handler.sa_flags     = SA_SIGINFO;
    res = sigaction(SIGUSR1, &timer_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGEV_SIGNAL handler %s", strerror(errno));
      status = FAILURE;
      break;
    }

    if (init_workers(num_workers) != SUCCESS)
    {
      LOG_ERROR("Could not initialize workers");
      res = FAILURE;
      break;
    }

    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }

    // Register callbacks for various messages
    if (register_cb(LIGHT_RSP, MAIN_TASK, light_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register light response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(IS_DARK_RSP, MAIN_TASK, is_dark_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register is dark response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(TEMP_RSP, MAIN_TASK, temp_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register temp response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(AIR_RSP, MAIN_TASK, air_rsp_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register air response handler");
      status = FAILURE;
      break;
    }

    if (register_cb(HEARTBEAT_SETUP, MAIN_TASK, hb_setup) != SUCCESS)
    {
      LOG_ERROR("Could not register heartbeat setup handler");
      status = FAILURE;
      break;
    }

    if (register_cb(HEARTBEAT, MAIN_TASK, hb_handler) != SUCCESS)
    {
      LOG_ERROR("Could not register heartbeat setup handler");
      status = FAILURE;
      break;
    }

    if (log_msg_init(file_name, 1) != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_temp(0) != SUCCESS)
    {
      LOG_ERROR("Could not initialize temp");
      status = FAILURE;
      break;
    }

    if (init_humidity(0) != SUCCESS)
    {
      LOG_ERROR("Could not initialize humidity");
      status = FAILURE;
      break;
    }

    if (init_air(0) != SUCCESS)
    {
      LOG_ERROR("Could not initialize air");
      status = FAILURE;
      break;
    }

    if (init_light(0) != SUCCESS)
    {
      LOG_ERROR("Could not initialize light");
      status = FAILURE;
      break;
    }

    if (server_init() != SUCCESS)
    {
      LOG_ERROR("Could not initialize server");
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

status_t dest_main_server_task()
{
  status_t status = SUCCESS;

  if (server_dest() != SUCCESS)
  {
    LOG_ERROR("Could not destroy server");
    status = FAILURE;
  }

  if (dest_temp(0) != SUCCESS)
  {
    LOG_ERROR("Could not destroy temp task");
    status = FAILURE;
  }

  if (dest_humidity(0) != SUCCESS)
  {
    LOG_ERROR("Could not destroy air task");
    status = FAILURE;
  }

  if (dest_air(0) != SUCCESS)
  {
    LOG_ERROR("Could not destroy air task");
    status = FAILURE;
  }

  if (dest_light(0) != SUCCESS)
  {
    LOG_ERROR("Could not destroy light task");
    status = FAILURE;
  }

  if (unregister_cb(LIGHT_RSP, MAIN_TASK, light_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister light response handler");
    status = FAILURE;
  }

  if (unregister_cb(IS_DARK_RSP, MAIN_TASK, is_dark_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister is dark response handler");
    status = FAILURE;
  }

  if (unregister_cb(TEMP_RSP, MAIN_TASK, temp_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister temp response handler");
    status = FAILURE;
  }

  if (unregister_cb(AIR_RSP, MAIN_TASK, air_rsp_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister air response handler");
    status = FAILURE;
  }

  if (unregister_cb(HEARTBEAT, MAIN_TASK, hb_handler) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat handler");
    status = FAILURE;
  }

  if (unregister_cb(HEARTBEAT_SETUP, MAIN_TASK, hb_setup) != SUCCESS)
  {
    LOG_ERROR("Could not unregister heartbeat setup handler");
    status = FAILURE;
  }

  if (dest_workers() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }

  del_timers();

#ifdef BBB
  if (status == FAILURE)
  {
    set_brightness(USR3_LED, ON_BRIGHTNESS);
  }
#endif // BBB
  log_destroy();
  return status;
}
