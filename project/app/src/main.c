/** @file main.c
*
* @brief Main Task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <mqueue.h>
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "workers.h"
#include "log.h"

// Abort signal for all threads
int abort_signal = 0;

void * print_cb(void * params)
{
  LOG_LOW("cb called");
  return NULL;
}

int main()
{
  init_workers();
  register_cb(LOG, print_cb);
  uint32_t a = 1;
  uint32_t b = 2;

  mqd_t msg_q = get_writeable_queue();
  for (int i = 0; i < 10000; i++)
  {
    mq_send(msg_q, (char *)&a, sizeof(a), 0);
    mq_send(msg_q, (char *)&b, sizeof(b), 0);
  }

  usleep(1000000);
  return 0;
}
