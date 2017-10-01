/** @file ex3prob3.c
*
* @brief Main function
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "log.h"
#include "project_defs.h"

#define STACK_SIZE (65536)
#define GUARD_SIZE (4096)

uint32_t abort_signal;
pthread_mutex_t mutex;
pthread_cond_t cond;
int32_t synchronized_data = 0;

/*!
* @brief Test function for pthread
* @param[in] param null pointer
* @return NULL
*/
void * test_func1(void * param)
{
  FUNC_ENTRY;
  int32_t ret;

  // Try to lock mutex for condition
  LOG_LOW("Locking mutex");
  ret = pthread_mutex_lock(&mutex);
  if (ret != 0)
  {
    // Bail out if condition failed
    LOG_ERROR("Could not thread attrs");
    pthread_exit(NULL);
  }
  else
  {
    // Wait for first thread to signal
    LOG_LOW("Waiting on condition");
    ret = pthread_cond_wait(&cond, &mutex);
    if (ret != 0)
    {
      LOG_ERROR("Could not wait for condition %s", strerror(ret));
      pthread_exit(NULL);
    }

    // Condition was received use sleep to synthesize work and increment
    // "synchronized data"
    LOG_LOW("Received condition");
    LOG_LOW("Sleeping for 5 seconds");
    usleep(5000000);
    synchronized_data++;

    // Unlock the mutex and exit
    LOG_LOW("Unlocking mutex");
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0)
    {
      LOG_ERROR("Could not lock mutex %s", strerror(ret));
      pthread_exit(NULL);
    }
  }
  pthread_exit(NULL);
}

/*!
* @brief Test function for pthread
* @param[in] param null pointer
* @return NULL
*/
void * test_func(void * param)
{
  FUNC_ENTRY;

  int32_t ret;
  size_t stacksize;
  void * stackaddr;
  pthread_attr_t attr;
  pthread_t self = pthread_self();

  // Get attributes to check stack addr and stack size
  ret = pthread_getattr_np(self, &attr);
  if (ret != 0)
  {
    LOG_ERROR("Could not thread attrs");
    return NULL;
  }

  // Get the stack addr and stack size and display
  ret = pthread_attr_getstack(&attr, &stackaddr, &stacksize);
  if (ret != 0)
  {
    LOG_ERROR("Could not get stack info");
    return NULL;
  }
  LOG_HIGH("Stack address %p Stack size %zu", stackaddr, stacksize);

  // Sleep for 2 seconds to give time for the other thread to get into the
  // wait conditiion stack
  LOG_LOW("Sleeping for 2 seconds");
  usleep(2000000);

  // Lock the mutex on signal if lock was a success
  LOG_LOW("Locking mutex");
  ret = pthread_mutex_lock(&mutex);
  if (ret == 0)
  {
    // Signal the conditation
    LOG_LOW("Signaling condition");
    ret = pthread_cond_signal(&cond);
    if (ret != 0)
    {
      LOG_ERROR("%s", strerror(ret));
    }

    // Unlock the mutex and sleep for .5 seconds to allow other thread to lock mutex
    LOG_LOW("Unlocking mutex");
    ret = pthread_mutex_unlock(&mutex);
    if (ret != 0)
    {
      LOG_ERROR("Could not lock mutex");
      pthread_exit(NULL);
    }
    usleep(500000);

    // Use try lock which should fail because the other thread has the lock
    ret = pthread_mutex_trylock(&mutex);
    if (ret != 0)
    {
      LOG_ERROR("Could not lock mutex (expected)");
    }
    LOG_LOW("synchronized_data on trylock %d", synchronized_data);

    // Use a normal lock which will wait and display data changed
    ret = pthread_mutex_lock(&mutex);
    if (ret != 0)
    {
      LOG_ERROR("Could not lock mutex (unexpected)");
      pthread_exit(NULL);
    }
    LOG_LOW("synchronized_data on lock %d", synchronized_data);
    pthread_mutex_unlock(&mutex);
  }
  pthread_exit(NULL);
}

/*!
* @brief Main function
* @return 0
*/
int main()
{
  int32_t ret;
  pthread_t test_thread;
  pthread_t test_thread1;
  pthread_attr_t test_attr;

  // Initialize log
  log_init();

  FUNC_ENTRY;

  // Initialize condition variable
  ret = pthread_cond_init(&cond, NULL);
  if (ret != 0)
  {
    LOG_ERROR("Could not init condition");
    return 1;
  }
  else
  {
    LOG_LOW("Initialized condition");
  }

  // Initialize mutex
  ret = pthread_mutex_init(&mutex, NULL);
  if (ret != 0)
  {
    LOG_ERROR("Could not init mutex");
    return 1;
  }
  else
  {
    LOG_LOW("Initialized mutex");
  }

  // Initialize pthread attr
  ret = pthread_attr_init(&test_attr);
  if (ret != 0)
  {
    LOG_ERROR("Could not init attr");
    return 1;
  }
  else
  {
    LOG_LOW("Initialized thread attributes");
  }

  // Set the stack size for pthread attr
  ret = pthread_attr_setstacksize(&test_attr, STACK_SIZE);
  if (ret != 0)
  {
    LOG_ERROR("Could not set stack size");
    return 1;
  }
  else
  {
    LOG_LOW("Initialized thread stack size to %d", STACK_SIZE);
  }

  // Set the guard size for pthread attr
  ret = pthread_attr_setguardsize(&test_attr, GUARD_SIZE);
  if (ret != 0)
  {
    LOG_ERROR("Could not set guard size");
    return 1;
  }
  else
  {
    LOG_LOW("Initialized thread guard size to %d", GUARD_SIZE);
  }

  // Create pthread
  ret = pthread_create(&test_thread, &test_attr, test_func, NULL);
  if (ret != 0)
  {
    LOG_ERROR("Could not set create pthread %s", strerror(ret));
    return 1;
  }
  else
  {
    LOG_LOW("Created thread");
  }

  // Create phtread
  ret = pthread_create(&test_thread1, &test_attr, test_func1, NULL);
  if (ret != 0)
  {
    LOG_ERROR("Could not set create pthread %s", strerror(ret));
    return 1;
  }
  else
  {
    LOG_LOW("Created thread");
  }

  LOG_HIGH("Waiting for threads to join");
  pthread_join(test_thread, NULL);
  pthread_join(test_thread1, NULL);
  LOG_HIGH("Threads joined");

  // Destroy condition
  pthread_cond_destroy(&cond);

  // Destroy mutex
  pthread_mutex_destroy(&mutex);

  // Destory attrs
  pthread_attr_destroy(&test_attr);

  // Destory log
  log_destroy();
  return 0;
}
