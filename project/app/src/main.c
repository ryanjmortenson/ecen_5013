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

int main()
{
  FUNC_ENTRY;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize everything
  log_init();
  init_workers();
  log_msg_init("output.log");

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
