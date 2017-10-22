/** @file main.c
*
* @brief Main Task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <mqueue.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>

#include "workers.h"
#include "log.h"
#include "log_msg.h"

// Abort signal for all threads
int abort_signal = 0;

void sigint_handler(int sig)
{
  FUNC_ENTRY;

  // Flush the queue otherwise system process deadlocks
  flush_queue();

  // Set the abort signal
  abort_signal = 1;
}

int main(int argc, char *argv[])
{
  FUNC_ENTRY;
  char * file_name;
  uint32_t num_workers = 1;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize log to print errors
  log_init();

  // The first argument is required which is the log name
  if (argc < 2)
  {
    LOG_ERROR("The log name is a required argument");
    exit(1);
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
      exit(1);
    }
  }

  // Initialize the rest
  init_workers(num_workers);
  log_msg_init(file_name);

  // Register signal handler
  sigaction(SIGINT, &int_handler, 0);
  while(!abort_signal)
  {
    SEND_LOG_HIGH("Test");
    SEND_LOG_MED("Test");
    SEND_LOG_LOW("Test");
  }

  // Destroy everything
  log_msg_dest();
  dest_workers();
  log_destroy();
  return 0;
}
