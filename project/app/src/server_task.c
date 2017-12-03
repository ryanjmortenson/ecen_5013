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

// Global abort flag
extern int32_t abort_signal;
static mqd_t msg_q;
int32_t newsockfd = -1;

#define SERVER_PORT (12345)
#define SOCKET_BACKLOG_LEN (5)

void * server_send_cb(void * param)
{
  CHECK_NULL2(param);
  message_t * in = (message_t *)param;
  int32_t res;

  if (newsockfd >= 0 && in->network_routed == 0)
  {
    in->network_routed = 1;
    res = socket_write(newsockfd, in, sizeof(*in));
    if (res < 0)
    {
      LOG_ERROR("Could not write to socket");
    }
  }
  return NULL;
}

PTHREAD_RETURN_TYPE connection_thread(void * param)
{
  int32_t res;
  message_t msg;
  LOG_MED("Spawned a new connection thread for fd: %d", newsockfd);

  while(!abort_signal)
  {
    res = socket_recv(newsockfd, &msg, sizeof(msg));
    if (res < 0)
    {
      LOG_ERROR("Could not received message, %s", strerror(errno));
      abort_signal = 1;
      break;
    }
    else
    {
      LOG_FATAL("Received message");
    }

    // Message is already filled out so don't overwrite current data
    if (send_msg(msg_q, &msg, NULL, 0) == FAILURE)
    {
      LOG_ERROR("Could not add message to queue");
      abort_signal = 1;
      break;
    }
  }
  PTHREAD_RETURN(NULL);
}

PTHREAD_RETURN_TYPE server_thread(void * param)
{
  FUNC_ENTRY;

  struct sockaddr_in serv_addr;
  struct sockaddr_in cli_addr;

  int32_t sockfd;
  int32_t res = 0;
  uint32_t clilen = sizeof(cli_addr);
  pthread_t connection_task;
  int32_t enable = 1;

  do
  {
    // Create a socket file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
      LOG_ERROR("Could not create socket fd, %s", strerror(errno));
      abort_signal = 1;
      break;
    }

    // Initialize server address
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    res = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    if (res < 0)
    {
      LOG_ERROR("Could not set sockopt, %s", strerror(errno));
      abort_signal = 1;
      break;
    }

    res = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (res < 0)
    {
      LOG_ERROR("Could not bind socket fd, %s", strerror(errno));
      abort_signal = 1;
      break;
    }

    LOG_HIGH("Bind Successful on port %d", SERVER_PORT);

    // Set the socket as a passive socket
    res = listen(sockfd, SOCKET_BACKLOG_LEN);
    if (res < 0)
    {
      LOG_ERROR("Could not create socket fd, %s", strerror(errno));
      abort_signal = 1;
      break;
    }
  } while(0);

  while(!abort_signal)
  {
    // Wait for incoming connections
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

    // Log connection stats
    LOG_HIGH("Accepted Connection from %d on port %d",
            cli_addr.sin_addr.s_addr,
            cli_addr.sin_port);

    res = pthread_create(&connection_task, NULL, connection_thread, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not create connection thread");
      abort_signal = 1;
      break;
    }
  }
  LOG_HIGH("server thread exiting");
  close(sockfd);
  PTHREAD_RETURN(NULL);
} // server_service()

uint32_t server_init()
{
  FUNC_ENTRY;
  pthread_t server_task;
  int32_t res = 0;
  status_t status = SUCCESS;

  do
  {
    res = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not set cancellability of server task, %s",
                strerror(res));
      status = FAILURE;
      break;
    }

    res = pthread_create(&server_task, NULL, server_thread, NULL);
    if (res < 0)
    {
      LOG_ERROR("Could not create server task, %s", strerror(res));
      status = FAILURE;
      break;
    }

    res = register_cb(UNROUTED, ALL_TASKS, server_send_cb);
    if (res < 0)
    {
      LOG_ERROR("Could not register call back");
      status = FAILURE;
      break;
    }

    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not get message queue");
      status = FAILURE;
      break;
    }
  } while (0);
  return status;
} // server_init()
