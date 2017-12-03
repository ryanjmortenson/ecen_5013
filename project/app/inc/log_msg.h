/** @file log_msg.h
*
* @brief A description of the module’s purpose.
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _LOG_MSG_H
#define _LOG_MSG_H

#include <sys/time.h>
#include "workers.h"

typedef struct log_msg {
  uint8_t level;
  char file_name[128];
  char function[32];
  uint32_t line_no;
  char message[256];
  struct timeval tv;
} log_msg_t;

/*!
* @brief Initiate the log message catching functionality
* @param[in] file_name file name of log file
* @return SUCCESS/FAILURE
*/
status_t log_msg_init(char * file_name, uint8_t print);

/*!
* @brief Destroy the log message catching functionality
* @return SUCCESS/FAILURE
*/
status_t log_msg_dest();

void send_log
(
  log_level_t level,
  char * file_name,
  const char * function,
  uint32_t line_no,
  task_id_t from,
  ...
);

#define SEND_LOG(level, from, ...) send_log(level,                 \
                                            (char *)__FILE__,      \
                                            __FUNCTION__,          \
                                            __LINE__,              \
                                            from,                  \
                                            __VA_ARGS__)

#define SEND_LOG_HIGH(...) SEND_LOG(LOG_LEVEL_HIGH, TASK_ID, __VA_ARGS__)
#define SEND_LOG_MED(...)  SEND_LOG(LOG_LEVEL_MEDIUM, TASK_ID, __VA_ARGS__)
#define SEND_LOG_LOW(...)  SEND_LOG(LOG_LEVEL_LOW, TASK_ID, __VA_ARGS__)
#define SEND_LOG_FUNC(...) SEND_LOG(LOG_LEVEL_FUNC, TASK_ID, __VA_ARGS__)
#define SEND_LOG_ERROR(...) SEND_LOG(LOG_LEVEL_ERROR, TASK_ID, __VA_ARGS__)
#define SEND_LOG_FATAL(...) SEND_LOG(LOG_LEVEL_FATAL, TASK_ID, __VA_ARGS__)
#define SEND_INIT_COMPLETE() SEND_LOG_HIGH("INIT COMPLETE");

#endif /* _LOG_MSG_H */
