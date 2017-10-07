/** @file child1.c
*
* @brief Hold the child1 functionality from hw3 prob5
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "child1.h"
#include "log.h"
#include "project_defs.h"

#define READ_BUF_SIZE (1024)
#define BUF_SIZE (READ_BUF_SIZE + 1)

extern int32_t abort_signal;
sem_t sigusr1;
file_info_t file_stats_info;
pthread_mutex_t file_stats_mutex = PTHREAD_MUTEX_INITIALIZER;

/*!
* @brief SIGUSR1 handler
* @param[in] sig signal posted
*/
void sigusr1_handler(int sig)
{
  // Log the signal was captured and post a semaphore for the
  // main part of the thread
  LOG_LOW("SIGUSR1 captured");
  sem_post(&sigusr1);
}

/*!
* @brief Prints current file stats
*/
void print_file_stats()
{
  // Lock the thread info mutex
  pthread_mutex_lock(&file_stats_mutex);

  LOG_HIGH("char: %d, words: %d, lines: %d",
           file_stats_info.chars,
           file_stats_info.words,
           file_stats_info.lines);

  // Unlock the thread info mutex
  pthread_mutex_unlock(&file_stats_mutex);

}

/*!
* @brief Calculates file info
* @param[out] info file info stats pointer to be filled out
* @param[in] buf buffer to calculate stats on
* @param[in] bufsize size of buffer
*/
static inline void get_info(file_info_t * info, char * buf, int32_t bufsize)
{
  char prev_char = '\0';
  info->chars += bufsize;

  for (int32_t i = 0; i < bufsize; i++)
  {
    if (*(buf + i) == '\n')
    {
      info->lines++;
    }
    if ((*(buf + i) == ' ' || *(buf + i) == '\n') && prev_char != ' ')
    {
      info->words++;
    }
    prev_char = *(buf + i);
  }
}

/*!
* @brief Gets the file info
* @param[in] fd file descriptor of file to calculate info
* @param[out] info file info stats pointer to be filled out
* @return SUCCESS or FAILURE
*/
int32_t get_file_info(int32_t fd, file_info_t * info)
{
  FUNC_ENTRY;
  char buf[BUF_SIZE] = {0};
  int32_t bytes;

  // Reset the file info struct
  memset(info, 0, sizeof(*info));

  // Sync contents of file
  fsync(fd);

  // Seek to the beginning of the file
  lseek(fd, 0, SEEK_SET);

  // Lock the thread info mutex
  pthread_mutex_lock(&file_stats_mutex);

  // Loop over file collecting stats
  bytes = read(fd, buf, READ_BUF_SIZE);
  get_info(info, buf, bytes);
  while (bytes > 0)
  {
    bytes = read(fd, buf, READ_BUF_SIZE);
    get_info(info, buf, bytes);
  }

  // Subtract off one to compensate for the EOF marker
  info->chars--;

  // Unlock the thread info mutex
  pthread_mutex_unlock(&file_stats_mutex);

  return SUCCESS;
}

/*!
* @brief Main child1 thread function
* @param[in] param pointer to file descriptor
* @return NULL
*/
void * child1_thread(void * param)
{
  FUNC_ENTRY;

  int fd = *(int *)param;

  while(1)
  {
    // Wait for signal handler to set semaphore
    sem_wait(&sigusr1);

    // Log the semphore was received
    LOG_LOW("Received SIGUSR1 semaphore");

    // Before proceeding check for abort signal
    if (abort_signal)
    {
      break;
    }

    // Get the file info
    get_file_info(fd, &file_stats_info);
  }

  // Clean up
  close(fd);
  free(param);
  sem_destroy(&sigusr1);

  return NULL;
}

/*!
* @brief Child1 initialization
* @param[in] file_name name of file to run stats on
* @param[out] child1 pointer to pthread data
* @return SUCCESS/FAILURE
*/
int32_t child1_init(char * file_name, pthread_t * child1)
{
  FUNC_ENTRY;

  int ret = 0;
  int * fd = NULL;
  struct sigaction usr1_handler = {.sa_handler=sigusr1_handler};

  // Try to malloc an integer for file descriptor
  fd = malloc(sizeof(*fd));
  if (fd == NULL)
  {
    LOG_ERROR("Could not malloc fd");
    return FAILURE;
  }

  // Register USR2 signal handler
  ret = sigaction(SIGUSR1, &usr1_handler, 0);
  if (ret < 0)
  {
    LOG_ERROR("Could not register SIGUSR1 signal handler: %s", strerror(errno));
    return FAILURE;
  }

  // Create the semaphore to be posted by the signal handler
  ret = sem_init(&sigusr1, 0, 0);
  if (ret < 0)
  {
    LOG_ERROR("Could not create child2 semaphore: %s", strerror(errno));
    return FAILURE;
  }

  // Try and open the file
  *fd = open(file_name, O_RDONLY);
  if (fd < 0)
  {
    LOG_ERROR("Could not open %s, errno: %s", file_name, strerror(errno));
    return FAILURE;
  }
  else
  {
    LOG_HIGH("Opened %s, fd %d", file_name, *fd);
  }

  // Create the pthread
  ret = pthread_create(child1, NULL, child1_thread, fd);
  if (ret < 0)
  {
    LOG_ERROR("Could not create child1 thread: %s", strerror(errno));
    return FAILURE;
  }

  return SUCCESS;
}
