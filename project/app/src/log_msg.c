/** @file log_msg.c
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>

#include "project_defs.h"
#include "log.h"
#include "log_msg.h"
#include "workers.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int fd = 0;
static mqd_t msg_q;

#define LOG_FMT "%-7s %-12s in [%20s] line %4u: %s\n"

void send_log
(
  log_level_t level,
  char * file_name,
  const char * function,
  uint32_t line_no,
  ...
)
{
  log_msg_t log;
  message_t msg;
  va_list var_args;
  char * fmt;
  char fmt_buffer[256];

  // Fill out the "easy" parts of the log message
  log.level = level;
  log.line_no = line_no;
  memcpy(log.file_name, file_name, 32);
  memcpy(log.function, function, 20);

  // File out the message porition by going through var args
  va_start(var_args, line_no);
  fmt = va_arg(var_args, char *);
  vsnprintf(fmt_buffer, 256, fmt, var_args);
  memcpy(log.message, fmt_buffer, 256);

  // Send the top level message
  msg.type = LOG;
  memcpy(&msg.msg, &log, sizeof(log));
  mq_send(msg_q, (char *)&msg, sizeof(msg), 0);
}

void * print_log(void * param)
{
  log_msg_t * log = (log_msg_t *)param;
  log_level(log->level,
            log->file_name,
            log->function,
            log->line_no,
            "[LOG_MSG] %s",
            log->message);
  return NULL;
}

void * write_log(void * param)
{
  log_msg_t * log = (log_msg_t *)param;
  char log_buf[256];
  size_t len;
  len = snprintf(log_buf,
                 256,
                 LOG_FMT,
                 get_log_leve_string(log->level),
                 get_basename(log->file_name, PATH_SEPARATOR),
                 log->function,
                 log->line_no,
                 log->message);
  int res = write(fd, log_buf, len + 1);

  LOG_ERROR("%d", res);
  return NULL;
}

status_t log_msg_init(char * file_name)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  status_t res;

  do
  {
    fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd < 0)
    {
      LOG_ERROR("Could not open file, %s", strerror(errno));
      status = FAILURE;
      break;
    }
    LOG_MED("Opened file %s", file_name);

    res = register_cb(LOG, print_log);
    res = register_cb(LOG, write_log);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not register callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    msg_q = get_writeable_queue();

  } while(0);
  return status;
}
