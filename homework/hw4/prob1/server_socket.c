/** @file client_socket.c
*
* @brief Holds functionality for socket client
*
*/

#include <errno.h>
#include <fcntl.h>
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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "user_space.h"

#define SERVER_PORT (12345)
#define SOCKET_BACKLOG_LEN (5)

uint8_t abort_flag = 0;
int32_t sockfd;
int32_t newsockfd;

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
}

/*!
* @brief Ensure to receive all bytes on a read
* @param sockfd socket file descriptor
* @param data location to store data
* @param count number of bytes to read
* @return number of bytes read
*/
static inline
uint32_t server_recv(int32_t sockfd, void * data, uint32_t count)
{
  int32_t bytes = 0;
  int32_t res = 0;

  printf("Trying to recv %d bytes\n", count);
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
} // server_recv()

/*!
* @brief Open file and socket looping over commands sent by client and
* responding
*/
int main()
{
  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;
  int32_t res;
  int32_t fd;
  uint32_t clilen;
  char buffer[256];
  int32_t len;
  struct sigaction int_handler = {.sa_handler=sigint_handler};

  // Register a handler to clean up on CTRL-C correctly
  res = sigaction(SIGINT, &int_handler, 0);
  if (res < 0)
  {
    printf("Could not register SIGINT handler %s\n", strerror(errno));
    return -1;
  }

  // Initialize server address
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(SERVER_PORT);

  // Create socket fd
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    printf("Could not create socket %s\n", strerror(errno));
    return -1;
  }

  // Set the socket options to you can immediately reuse port
  int enable = 1;
  res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
  if (res == -1)
  {
    printf("Could not set sockopt %s", strerror(errno));
    return -1;
  }

  // Take control of port
  res = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (res == -1)
  {
    printf("Could not bind to port %s\n", strerror(errno));
    return -1;
  }

  // Set the listen parameters
  res = listen(sockfd, SOCKET_BACKLOG_LEN);
  if (res == -1)
  {
    printf("Could not set socket for listening %s", strerror(errno));
    return -1;
  }

  // Accept incoming connection
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if (newsockfd == -1)
  {
    printf("Could not accept incoming connection %s", strerror(errno));
    return -1;
  }

  // Notify user of connection
  printf("Accepted connection from %d on port %d\n",
         cli_addr.sin_addr.s_addr,
         cli_addr.sin_port);

  // Open the led driver
  fd = open(DEVICE, O_RDWR);
  if (fd < 0)
  {
    printf("Couldn't open %s\n", DEVICE);
    return -1;
  }

  // Loop over commands from client executing and responding
  while(!abort_flag)
  {
    cmd_t cmd;

    // Receive the length of the next command
    res = server_recv(newsockfd, buffer, 4);
    if (res == -1)
    {
      printf("Could not receive 4 bytes %s\n", strerror(errno));
      break;
    }

    // Receive the command
    len = *(uint32_t *)buffer;
    res = server_recv(newsockfd, buffer, len);
    if (res == -1)
    {
      printf("Could not receive %d bytes %s\n", len, strerror(errno));
      break;
    }

    // Execute command
    cmd = *(cmd_t *)buffer;
    res = run_cmd_ext(fd, &cmd, buffer, BUF_SIZE);
    if (res == -1)
    {
      printf("Could not run command");
      break;
    }

    // Send the length of the response
    len = strlen(buffer);
    res = write(newsockfd, (char *)&len, sizeof(len));
    if (res == -1)
    {
      printf("Could not send 4 bytes %s\n", strerror(errno));
      break;
    }

    // Send the response
    res = write(newsockfd, buffer, len);
    if (res == -1)
    {
      printf("Could not send %d bytes %s\n", len, strerror(errno));
      break;
    }
  }

  // Shutdown initial socket
  res = shutdown(sockfd, SHUT_RDWR);
  if (res == -1)
  {
    printf("Could not shut down socket correctly %s\n", strerror(errno));
    return -1;
  }

  // Shutdown created socket
  res = shutdown(newsockfd, SHUT_RDWR);
  if (res == -1)
  {
    printf("Could not shut down socket correctly %s\n", strerror(errno));
    return -1;
  }
  return 0;
}
