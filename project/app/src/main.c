/** @file main.c
*
* @brief Main function
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "child1.h"
#include "child2.h"
#include "log.h"
#include "project_defs.h"

uint32_t abort_signal = 0;
volatile sig_atomic_t signal_interrupted = 0;

void sigint_handler(int sig)
{
  LOG_FATAL("SIGINT captured");
  signal_interrupted = 1;
}

int main(int argc, char *argv[])
{
  int fd;
  int ret;
  char input;
  char * file_name = argv[1];
  pthread_t child1;
  pthread_t child2;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Initialize log
  log_init();

  FUNC_ENTRY;

  if (argc != 2)
  {
    LOG_ERROR("Only 1 parameter (file name) is required, you provided %d", argc - 1);
    return 1;
  }

  // Display file name
  LOG_LOW("File name: %s", file_name);

  // Try and open the file
  fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0766);
  if (fd < 0)
  {
    LOG_ERROR("Could not open %s, errno: %s", file_name, strerror(errno));
    return 1;
  }
  else
  {
    LOG_HIGH("Opened %s, fd %d", file_name, fd);
  }

  // Initialize children threads
  ret = child1_init(file_name, &child1);
  if (ret != SUCCESS)
  {
    LOG_ERROR("Child 1 creation failed");
    return 1;
  }

  ret = child2_init(&child2);
  if (ret != SUCCESS)
  {
    LOG_ERROR("Child 2 creation failed");
    return 1;
  }

  // Register signal handler
  sigaction(SIGINT, &int_handler, 0);

  // Loop until ctrl-c is pressed
  while (!signal_interrupted)
  {
    input = getchar();
    ret = write(fd, &input, sizeof(input));
    if (ret < 0)
    {
      LOG_ERROR("Could not write data %c to file %s", input, file_name);
    }
  }

  // Set the abort flag for all other threads
  abort_signal = 1;

  // Send the two signals to get threads to break out of their loops
  kill(getpid(), SIGUSR2);
  kill(getpid(), SIGUSR1);

  // Wait for threads to hoin
  pthread_join(child1, NULL);
  LOG_HIGH("child1 joined");
  pthread_join(child2, NULL);
  LOG_HIGH("child2 joined");

  // Close file
  ret = close(fd);
  if (ret < 0)
  {
    LOG_ERROR("Could not close %s, errno: %s", file_name, strerror(errno));
  }

  // Destory log
  log_destroy();
  return 0;
}
