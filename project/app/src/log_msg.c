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
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

#include "project_defs.h"
#include "log.h"
#include "log_msg.h"
#include "workers.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int fd = 0;
static mqd_t msg_q;

#define LOG_FMT "%-6s %-10.10s [%10.10s] %4u: %s\n"
#define LOG_BUFFER_MAX (256)
#define FILE_NAME_BUF_MAX (32)
#define FUNCTION_MAX (20)

/*!
* @brief Send a log message via message queue
* @param[in] level logging level for this statement
* @param[in] file_name pointer to the file name
* @param[in] function pointer to the function name
* @param[in] line_no line number in file
* @param[in] ... variadic arguments for printf
*/
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
  char fmt_buffer[LOG_BUFFER_MAX];

  // Fill out the "easy" parts of the log message
  log.level = level;
  log.line_no = line_no;
  memcpy(log.file_name, file_name, FILE_NAME_BUF_MAX);
  memcpy(log.function, function, FUNCTION_MAX);

  // File out the message porition by going through var args
  va_start(var_args, line_no);
  fmt = va_arg(var_args, char *);
  vsnprintf(fmt_buffer, FILE_NAME_BUF_MAX, fmt, var_args);
  memcpy(log.message, fmt_buffer, FILE_NAME_BUF_MAX);

  // Send the top level message
  gettimeofday(&log.tv, NULL);
  msg.type = LOG;
  memcpy(&msg.msg, &log, sizeof(log));
  mq_send(msg_q, (char *)&msg, sizeof(msg), 0);
}

/*!
* @brief Print a log message caught on message queue
* @param[in] param log msg struct
* @return NULL
*/
void * print_log(void * param)
{
  log_msg_t * log = (log_msg_t *)param;
  char ts[TIMESTAMP_LEN];
  create_timestamp(&log->tv, ts);
  log_level(log->level,
            ts,
            log->file_name,
            log->function,
            log->line_no,
            "M (%lu) %s",
            pthread_self(),
            log->message);
  return NULL;
}

/*!
* @brief Write log message to log file
* @param[in] param log msg struct
* @return NULL
*/
void * write_log(void * param)
{
  log_msg_t * log = (log_msg_t *)param;
  char log_buf[LOG_BUFFER_MAX];
  size_t len;
  len = snprintf(log_buf,
                 LOG_BUFFER_MAX,
                 LOG_FMT,
                 get_log_level_string(log->level),
                 get_basename(log->file_name, PATH_SEPARATOR),
                 log->function,
                 log->line_no,
                 log->message);
  int res = write(fd, log_buf, len + 1);

  if (res < 0)
  {
    LOG_ERROR("Failed to write log");
  }
  return NULL;
}

/*!
* @brief Initiate the log message catching functionality
* @param[in] file_name file name of log file
* @return SUCCESS/FAILURE
*/
status_t log_msg_init(char * file_name)
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  status_t res;

  do
  {
    // Open file for writing
    fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    if (fd < 0)
    {
      LOG_ERROR("Could not open file, %s", strerror(errno));
      status = FAILURE;
      break;
    }
    LOG_MED("Opened file %s", file_name);

    // Register call backs for both printing and writing logs
    res = register_cb(LOG, print_log);
    res = register_cb(LOG, write_log);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not register callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Get a writeable queue
    msg_q = get_writeable_queue();
    if (msg_q < 0)
    {
      LOG_ERROR("Could not open message queue, %s", strerror(errno));
      status = FAILURE;
      break;
    }
  } while(0);
  return status;
}
