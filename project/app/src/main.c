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
  LOG_FATAL("Sigint handler");
  flush_queue();
  abort_signal = 1;
}

int main()
{
  log_init();
  init_workers();
  log_msg_init("output.log");
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Register signal handler
  sigaction(SIGINT, &int_handler, 0);
  while(!abort_signal)
  {
    SEND_LOG_HIGH("Test");
    SEND_LOG_MED("Test");
    SEND_LOG_LOW("Test");
  }
  LOG_FATAL("Broke out of loop");
  dest_workers();
  return 0;
}
