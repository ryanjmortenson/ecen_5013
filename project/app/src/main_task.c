/** @file main_task.c
*
* @brief Main project tasks
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include "light.h"
#include "log.h"
#include "log_msg.h"
#include "main_task.h"
#include "project_defs.h"
#include "temp.h"
#include "workers.h"

// Abort signal for all threads
int abort_signal = 0;

void sigint_handler(int sig)
{
  FUNC_ENTRY;

  // Set the abort signal
  abort_signal = 1;

  // Flush queue
  flush_queue();
}

status_t init_main_task(int argc, char *argv[])
{
  FUNC_ENTRY;
  char * file_name;
  uint32_t num_workers = 1;
  status_t status = SUCCESS;
  int32_t res;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize log to print errors
  log_init();

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
      res = FAILURE;
      break;
    }

    if (log_msg_init(file_name) != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_temp() != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    if (init_light() != SUCCESS)
    {
      LOG_ERROR("Could not initialize logging");
      status = FAILURE;
      break;
    }

    // Register signal handler
    res = sigaction(SIGINT, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT hander %s", strerror(errno));
      status = FAILURE;
      break;
    }

    sigaction(SIGTERM, &int_handler, 0);
    if (res < 0)
    {
      LOG_ERROR("Could not register SIGINT hander %s", strerror(errno));
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}

status_t dest_main_task()
{
  status_t status = SUCCESS;

  if (dest_light() != SUCCESS)
  {
    LOG_ERROR("Could not destory light task");
    status = FAILURE;
  }

  if (dest_temp() != SUCCESS)
  {
    LOG_ERROR("Could not destory temp task");
    status = FAILURE;
  }

  if (log_msg_dest() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }

  if (dest_workers() != SUCCESS)
  {
    LOG_ERROR("Could not destory log task");
    status = FAILURE;
  }

  log_destroy();
  return status;
}

void main_task()
{
  int count = 0;
  while(!abort_signal)
  {
    get_temp_f(count % 2, MAIN_TASK);
    send_light_req(count % 2, MAIN_TASK);
    count++;
    usleep(5000000);
  }
}

