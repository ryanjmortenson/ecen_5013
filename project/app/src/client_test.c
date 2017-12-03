#include <stdint.h>
#include <unistd.h>

#include "log.h"
#include "client_task.h"
#include "log_msg.h"
#include "workers.h"
#include "log_msg.h"

#define TASK_ID (CLIENT_TASK)

int main()
{
  FUNC_ENTRY;
  init_workers(1);
  log_msg_init(NULL, 0);
  client_init();
  while(1)
  {
    SEND_LOG_FATAL("woop");
    usleep(1000000);
  }
  return 0;
}
