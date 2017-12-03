#include <time.h>
#include <stdint.h>
#include <string.h>

#ifndef TIVA
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#else
#undef LWIP_TIMEVAL_PRIVATE
#include "FreeRTOS.h"
#include "task.h"
#include "pthread_wrapper.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#define connect(type, addr, size) lwip_connect(type, addr, size)
#define socket(family, type, other) lwip_socket(family, type, other)
#define read(fd, data, len) lwip_read(fd, data, len)
#define shutdown(fd, flags) lwip_shutdown(fd, flags)
#endif

#include "log.h"
#include "log_msg.h"
#include "project_defs.h"
#include "socket_helper.h"
#include "workers.h"

#define SERVER_PORT (12345)
#define ADDRESS "10.0.0.21"

// Global abort flag
extern int32_t abort_signal;

//static const task_id_t TASK_ID = CLIENT_TASK;
int32_t sockfd = -1;

void * client_send_cb(void * param)
{
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  int32_t res;

  if (sockfd > 0)
  {
    res = socket_write(sockfd, in, sizeof(*in));
    if (res < 0)
    {
      LOG_ERROR("Could not write to socket");
    }
  }
  else
  {
    LOG_ERROR("Socket fd is not initialized");
  }
  return NULL;
}

/*!
* @brief Shutdown socket
* @param sockfd socket file descriptor
* @return SUCCESS/FAILURE
*/
status_t client_dest(int32_t sockfd)
{
  FUNC_ENTRY;

  int32_t res = 0;

  // Shutdown the socket
  LOG_MED("Shutting down socket fd: %d", sockfd);
  EQ_RET_E(res, shutdown(sockfd, SHUT_RDWR), -1, FAILURE);

  return SUCCESS;
} // client_dest()

/*!
* @brief Handles incoming messages on queue
* @param param no data
* @return NULL
*/
PTHREAD_RETURN_TYPE client_thread(void * param)
{
  FUNC_ENTRY;
  int32_t res = 0;
  message_t msg;

  while(!abort_signal)
  {
    // Receive the length of messages
    res = socket_recv(sockfd, &msg, sizeof(msg));
    if (res < 0)
    {
      LOG_ERROR("Could not receive data length, %s", strerror(errno));
      abort_signal = 1;
      break;
    }
    else
    {
      LOG_FATAL("Received message");
    }
  }
  LOG_HIGH("client_service exiting");
  PTHREAD_RETURN(NULL);
} // client_service()

uint32_t client_init()
{
  FUNC_ENTRY;
  struct hostent * server;
  struct sockaddr_in serv_addr;
  int32_t res = 0;
  status_t status = SUCCESS;
  pthread_t client_task;

  // Memset the serv_addr struct to 0
  memset((void *)&serv_addr, 0, sizeof(serv_addr));

  do
  {
    // Instantiate socket and get the host name for connection
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
      LOG_ERROR("Could not create socket, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    server = gethostbyname(ADDRESS);
    if (server == NULL)
    {
      LOG_ERROR("Could not get host name, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Set up the serv_addr structure for connection
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(SERVER_PORT);

    LOG_HIGH("Trying connection to %s on port %d", ADDRESS, SERVER_PORT);
    res = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (res < 0)
    {
      LOG_ERROR("Could not connect, %s", strerror(errno));
      status = FAILURE;
      break;
    }
    LOG_HIGH("Connection successful");

    res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not set cancellability of client task, %s",
                strerror(res));
      status = FAILURE;
      break;
    }

    res = pthread_create(&client_task, NULL, client_thread, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not create client task, %s", strerror(res));
      status = FAILURE;
      break;
    }

    res = register_cb(LOG, LOG_TASK, client_send_cb);
    if (res < 0)
    {
      LOG_ERROR("Could not register call back");
      status = FAILURE;
      break;
    }
  } while (0);
  return status;
} // client_init()
