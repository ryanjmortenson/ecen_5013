/** @file ex3prob3.c
*
* @brief Main function
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#include "log.h"
#include "project_defs.h"

uint32_t abort_signal;

/*!
* @brief Test function for pthread
* @param[in] param null pointer
* @return NULL
*/
void * test_func(void * param)
{
  return NULL;
}

/*!
* @brief Main function
* @return 0
*/
int main()
{
  uint32_t ret;
  pthread_t test_thread;
  clock_t start;
  clock_t stop;
  double cpu_time_used;

  // Initialize log
  log_init();

  FUNC_ENTRY;

  // Test the time it takes for pthread to be created
  start = clock();
  pthread_create(&test_thread, NULL, test_func, NULL);
  stop = clock();
  cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;
  LOG_FATAL("CPU time used pthread: %f seconds", cpu_time_used);

  // Test the time it takes for process to be created
  start = clock();
  ret = fork();
  stop = clock();
  if (ret != 0)
  {
    cpu_time_used = ((double) (stop - start)) / CLOCKS_PER_SEC;
    LOG_FATAL("CPU time used fork: %f seconds", cpu_time_used);
  }

  // Destory log
  log_destroy();
  return 0;
}
