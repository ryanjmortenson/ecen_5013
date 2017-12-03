#include <stdint.h>
#include <unistd.h>

#include "log.h"
#include "log_msg.h"
#include "server_task.h"
#include "workers.h"

int main()
{
  FUNC_ENTRY;
  init_workers(5);
  server_init();
  log_msg_init("server_test.log", 1);
  while(1)
  {
    usleep(1000000);
  }
  return 0;
}
