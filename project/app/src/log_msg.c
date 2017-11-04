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

char * staleness_str[] = {
  "STALENESS_NEW",
  "STALENESS_OLD"
};

char * task_str[] = {
  [ALL_TASKS] = "ALL_TASKS",
  [MAIN_TASK] = "MAIN_TASK",
  [LIGHT_TASK] = "LIGHT_TASK",
  [TEMP_TASK] = "TEMP_TASK",
  [LOG_TASK] = "LOG_TASK",
  [TEST_TASK] = "TEST_TASK"
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int fd = 0;
static mqd_t msg_q;

// Log format Timestamp, level, file, function, line no, pthread id, message
#define LOG_FMT "%s %-6s %-10.10s [%10.10s] %4u: (%10.10s) %s\n"
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
  task_id_t from,
  ...
)
{
  log_msg_t log;
  va_list var_args;
  char * fmt;
  char fmt_buffer[LOG_BUFFER_MAX];
  message_t msg = MSG_INIT(LOG, LOG_TASK, from);

  if (msg_q > 0)
  {
    // Fill out the "easy" parts of the log message
    log.level = level;
    log.line_no = line_no;
    memcpy(log.file_name, file_name, FILE_NAME_BUF_MAX);
    memcpy(log.function, function, FUNCTION_MAX);

    // File out the message porition by going through var args
    va_start(var_args, from);
    fmt = va_arg(var_args, char *);
    vsnprintf(fmt_buffer, LOG_BUFFER_MAX, fmt, var_args);
    memcpy(log.message, fmt_buffer, LOG_BUFFER_MAX);

    // Send the top level message
    gettimeofday(&log.tv, NULL);
    if (send_msg(msg_q, &msg, &log, sizeof(log)))
    {
      LOG_ERROR("Could not send log message");
    }
  }
}

/*!
* @brief Print a log message caught on message queue
* @param[in] param log msg struct
* @return NULL
*/
static void * print_log(void * param)
{
  message_t * msg = (message_t *)param;
  log_msg_t * log = (log_msg_t *)msg->msg;
  char ts[TIMESTAMP_LEN];
  create_timestamp(&log->tv, ts);
  if (log->level < LOG_LEVEL || log->level > LOG_LEVEL_FUNC)
  {
    log_level(log->level,
              ts,
              log->file_name,
              log->function,
              log->line_no,
              "(%10.10s) %s",
              task_str[msg->from],
              log->message);
  }
  return NULL;
}

/*!
* @brief Write log message to log file
* @param[in] param log msg struct
* @return NULL
*/
static void * write_log(void * param)
{
  message_t * msg = (message_t *)param;
  log_msg_t * log = (log_msg_t*)msg->msg;
  char log_buf[LOG_BUFFER_MAX];
  size_t len;
  char ts[TIMESTAMP_LEN];
  create_timestamp(&log->tv, ts);
  len = snprintf(log_buf,
                 LOG_BUFFER_MAX,
                 LOG_FMT,
                 ts,
                 get_log_level_string(log->level),
                 get_basename(log->file_name, PATH_SEPARATOR),
                 log->function,
                 log->line_no,
                 task_str[msg->from],
                 log->message);

  if (fd > 0)
  {
    int res = write(fd, log_buf, len + 1);
    if (res < 0)
    {
      LOG_ERROR("Failed to write log");
    }
  }
  return NULL;
}

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
    res = register_cb(LOG, LOG_TASK, write_log);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not register callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    res = register_cb(LOG, LOG_TASK, print_log);
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

status_t log_msg_dest()
{
  FUNC_ENTRY;
  status_t status = SUCCESS;
  status_t res;

  do
  {
    // Close queue
    res = mq_close(msg_q);
    if (close < 0)
    {
      LOG_ERROR("Could not close message queue, %s", strerror(errno));
      status = FAILURE;
      break;
    }
    msg_q = -1;

    // Unegister call backs for printing
    res = unregister_cb(LOG, LOG_TASK, print_log);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not unregister callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Unegister call backs for writing log
    res = unregister_cb(LOG, LOG_TASK, write_log);
    if (res == FAILURE)
    {
      LOG_ERROR("Could not register callback, %s", strerror(errno));
      status = FAILURE;
      break;
    }

    // Close file
    res = close(fd);
    if (fd < 0)
    {
      LOG_ERROR("Could not open file, %s", strerror(errno));
      status = FAILURE;
      break;
    }
    fd = -1;
  } while(0);
  return status;
}
