/** @file child2.c
*
* @brief Hold the child2 functionality from hw3 prob5
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>

#include "child1.h"
#include "child2.h"
#include "log.h"
#include "project_defs.h"

extern int32_t abort_signal;
sem_t sigusr2;

/*!
* @brief SIGUSR2 handler
* @param[in] sig signal posted
*/
void sigusr2_handler(int sig)
{
  // Log the signal was captured and post a semaphore for the
  // main part of the thread
  LOG_LOW("SIGUSR2 captured");
  sem_post(&sigusr2);
}

/*!
* @brief Main child2 thread function
* @param[in] param not used
* @return NULL
*/
void * child2_thread(void * param)
{
  FUNC_ENTRY;

  // Loop waiting for signal
  while(1)
  {
    sem_wait(&sigusr2);
    LOG_LOW("Received SIGUSR2 semaphore");

    if (abort_signal)
    {

      break;
    }

    // Print the file stats
    print_file_stats();
  }

  sem_destroy(&sigusr2);
  return NULL;
}

int32_t child2_init(pthread_t * child2)
{
  FUNC_ENTRY;

  int ret = 0;
  struct sigaction usr2_handler = {.sa_handler=sigusr2_handler};

  // Register USR2 signal handler
  ret = sigaction(SIGUSR2, &usr2_handler, 0);
  if (ret < 0)
  {
    LOG_ERROR("Could not register SIGUSR2 signal handler: %s", strerror(errno));
    return FAILURE;
  }

  // Create the semaphore to be posted by the signal handler
  ret = sem_init(&sigusr2, 0, 0);
  if (ret < 0)
  {
    LOG_ERROR("Could not create child2 semaphore: %s", strerror(errno));
    return FAILURE;
  }

  // Create the thread
  ret = pthread_create(child2, NULL, child2_thread, NULL);
  if (ret < 0)
  {
    LOG_ERROR("Could not create child2 thread: %s", strerror(errno));
    return FAILURE;
  }

  return SUCCESS;
}
