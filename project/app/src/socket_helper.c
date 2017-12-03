#include <stdint.h>
#include <unistd.h>

#include "log.h"
#ifdef TIVA
#include "FreeRTOS.h"
#include "task.h"
#include "pthread_wrapper.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#define read(fd, data, len) lwip_read(fd, data, len)
#define write(fd, data, len) lwip_write(fd, data, len)
#endif // TIVA

int32_t socket_recv(int32_t sockfd, void * data, uint32_t count)
{
  FUNC_ENTRY;

  int32_t bytes = 0;
  int32_t res = 0;

  while (bytes < count)
  {
    res = read(sockfd, data + bytes, count - bytes);
    if (res < 0)
    {
      break;
    }
    bytes += res;
  }
  return bytes;
}

int32_t socket_write(int32_t sockfd, void * data, uint32_t count)
{
  FUNC_ENTRY;

  int32_t bytes = 0;
  int32_t res = 0;

  while (bytes < count)
  {
    res = write(sockfd, data + bytes, count - bytes);
    if (res < 0)
    {
      break;
    }
    bytes += res;
  }
  return bytes;
}
