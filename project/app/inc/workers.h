/** @file workers.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/
#include <mqueue.h>

#include "project_defs.h"

#define WORKER_QUEUE "/worker"

typedef struct message {
  type_t type;
  char msg[512];
} message_t;

status_t init_workers();
status_t dest_workers();
mqd_t get_writeable_queue();
status_t register_cb(type_t type, CALLBACK cb);
status_t flush_queue();
