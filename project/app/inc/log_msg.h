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

typedef struct log_msg {
  log_level_t level;
  char file_name[128];
  char function[32];
  uint32_t line_no;
  char message[256];
  struct timeval tv;
} log_msg_t;

status_t log_msg_init(char * file_name);

void send_log
(
  log_level_t level,
  char * file_name,
  const char * function,
  uint32_t line_no,
  ...
);

#define SEND_LOG(level, ...) send_log(level,                 \
                                      (char *)__FILE__,      \
                                      __FUNCTION__,          \
                                      __LINE__,              \
                                      __VA_ARGS__)


// Different log levels which can be turned on/off by setting LOG_LEVEL
#if LOG_LEVEL > 0
#define SEND_LOG_HIGH(...) SEND_LOG(LOG_LEVEL_HIGH, __VA_ARGS__)
#else
#define SEND_LOG_HIGH(...)
#endif /* LOG_LEVEL > 0 */

#if LOG_LEVEL > 1
#define SEND_LOG_MED(...)  SEND_LOG(LOG_LEVEL_MEDIUM, __VA_ARGS__)
#else
#define SEND_LOG_MED(...)
#endif  /* LOG_LEVEL > 1 */

#if LOG_LEVEL > 2
#define SEND_LOG_LOW(...)  SEND_LOG(LOG_LEVEL_LOW, __VA_ARGS__)
#else
#define SEND_LOG_LOW(...)
#endif  /* LOG_LEVEL > 2 */

#if LOG_LEVEL > 3
#define SEND_LOG_FUNC(...) SEND_LOG(LOG_LEVEL_FUNC, __VA_ARGS__)
#else
#define SEND_LOG_FUNC(...)
#endif  /* LOG_LEVEL > 2 */

// Error and Fatal definitions.  Fatal should be used sparingly and
// mostly debugging
#define SEND_LOG_ERROR(...) SEND_LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define SEND_LOG_FATAL(...) SEND_LOG(LOG_LEVEL_FATAL, __VA_ARGS__)

#endif /* _LOG_MSG_H */
