/** @file client_socket.c
*
* @brief Holds functionality for socket client
*
*/

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>

#include "user_space.h"
#include "led_module.h"

#define SERVER_PORT (12345)
#define SERVER_ADDRESS "10.0.0.39"

uint8_t abort_flag = 0;
int32_t sockfd;

// An initial command that will be changed throughout testing
static cmd_t cmd0 = {
  .period_ms = 500,
  .duty_cycle = 25,
  .read_command = READ_ALL,
  .blink = 0
};

/*!
* @brief Handle sigint and sigterm signals to shutdown gracefully
* @param sig signal that invoke handler
*/
void sigint_handler(int sig)
{
  int32_t res;
  abort_flag = 1;
  res = shutdown(sockfd, SHUT_RDWR);
  if (res == -1)
  {
    printf("Could not shut down socket correctly %s\n", strerror(errno));
    exit(1);
  }
  close(sockfd);
  exit(0);
}

/*!
* @brief Ensure to receive all bytes on a read
* @param sockfd socket file descriptor
* @param data location to store data
* @param count number of bytes to read
* @return number of bytes read
*/
static inline
uint32_t client_recv(int32_t sockfd, void * data, uint32_t count)
{
  int32_t bytes = 0;
  int32_t res = 0;

  printf("Trying to read %d bytes\n", count);
  while (bytes < count)
  {
    res = read(sockfd, data + bytes, count - bytes);
    if (res == -1)
    {
      printf("Could not read all the bytes %s\n", strerror(errno));
      return -1;
    }
    bytes += res;
  }
  return bytes;
} // client_recv()

/*!
* @brief Run a command and get current status of led module
* @param command to run
* @return status of running command
*/
static int32_t run_cmd(int fd, cmd_t * cmd)
{
  int32_t res;
  int32_t len = sizeof(*cmd);
  char buffer[BUF_SIZE];

  print_cmd(cmd);
  res = write(fd, (char *)&len, 4);
  printf("Wrote bytes %d\n", res);
  if (res < 0)
  {
    printf("Failed to write length\n");
    return -1;
  }

  res = write(fd, (char *)cmd, len);
  printf("Wrote bytes %d\n", res);
  if (res < 0)
  {
    printf("Failed to write command\n");
    return -1;
  }

  res = client_recv(fd, (char *)&len, sizeof(len));
  if (res < 0)
  {
    printf("Failed to read to led module\n");
    return -1;
  }

  memset(buffer, 0, BUF_SIZE);
  res = client_recv(fd, buffer, len);
  if (res < 0)
  {
    printf("Could not read response\n");
    return -1;
  }
  print_rsp(buffer);
}


int main()
{
  struct hostent * server;
  struct sockaddr_in serv_addr;
  int32_t res = 0;
  int32_t name_len = 0;
  int32_t buf_len = 0;
  char buffer[256];
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Memset the serv_addr struct to 0
  memset((void *)&serv_addr, 0, sizeof(serv_addr));

  // Register a handler to clean up on CTRL-C correctly
  res = sigaction(SIGINT, &int_handler, 0);
  if (res < 0)
  {
    printf("Could not register SIGINT handler %s\n", strerror(errno));
    return -1;
  }

  // Create socket fd
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Could not create socket %s\n", strerror(errno));
    return -1;
  }

  // Get the host name
  server = gethostbyname(SERVER_ADDRESS);
  if (server == NULL)
  {
    printf("Could not get host name %s\n", strerror(errno));
    return -1;
  }

  // Set up server addres structures
  serv_addr.sin_family = AF_INET;
  memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
  serv_addr.sin_port = htons(SERVER_PORT);

  // Connect
  printf("Trying connection to %s on port %d\n", SERVER_ADDRESS, SERVER_PORT);
  res = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
  if (res == -1)
  {
    printf("Could not connect %s\n", strerror(errno));
    return -1;
  }

  // Run a correctly formatted command
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted command with blink on
  cmd0.blink = 1;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted command with blink off and read blink state
  cmd0.blink = 0;
  cmd0.read_command = READ_BLINK_STATE;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read period
  cmd0.read_command = READ_PERIOD;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read duty cycle
  cmd0.read_command = READ_DUTY_CYCLE;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and read led state
  cmd0.blink = 1;
  cmd0.period_ms = 1000;
  cmd0.duty_cycle = 50;
  cmd0.read_command = READ_LED_STATE;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  // Run a correctly formatted and all
  cmd0.read_command = READ_ALL;
  if (run_cmd(sockfd, &cmd0) == -1)
  {
    return -1;
  }

  return 0;
} // main()
