#include <stdint.h>
#include <unistd.h>

#include "log.h"
#include "log_msg.h"
#include "server_task.h"
#include "workers.h"

void * cb(void * param)
{
  LOG_FATAL("Received a message");
  return NULL;
}

int main()
{
  FUNC_ENTRY;
  init_workers(1);
  server_init();
  log_msg_init("server_test.log", 1);
  register_cb(LOG, LOG_TASK, cb);
  while(1)
  {
    usleep(1000000);
  }
  return 0;
}
