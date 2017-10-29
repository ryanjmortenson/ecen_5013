/** @file log.h
*
* @brief Defines items used in logging
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <sys/time.h>

// Logging level enumerations
typedef enum {
  LOG_LEVEL_HIGH,
  LOG_LEVEL_MEDIUM,
  LOG_LEVEL_LOW,
  LOG_LEVEL_FUNC,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_FATAL
} log_level_t;

#define PATH_SEPARATOR "/"
#define TIMESTAMP_LEN (20)

/*!
* @brief Initializes the syslog
* @return nothing
*/
void log_init();

/*!
* @brief Destroy syslog
* @return nothing
*/
void log_destroy();

/*!
* @brief Get the base file name from a string like app/filename.c
* @param[in] p_filename pointer to the file name
* @param[in] p_path_seperator how the path is seperated
* @return pointer to the basename
*/
char * get_basename(char * p_filename, const char * p_path_seperator);

/*!
* @brief Convert the log level enum to string
* @param[in] level log level
* @return pointer to the log level string
*/
const char * get_log_level_string(log_level_t level);

/*!
* @brief Convert timeval struct into stimestamp string
* @param[in] tv timeval struct holding time for conversion
* @param[in] buf buffer to holding the converted string
*/
void create_timestamp(struct timeval * tv, char * buf);


/*!
* @brief Log at a certain level
* @param[in] level logging level for this statement
* @param[in] timestamp timestamp string, if NULL will create timestamp
* @param[in] p_filename pointer to the file name
* @param[in] p_function pointer to the function name
* @param[in] line_no line number in file
* @param[in] ... variadic arguments for printf
*/
void log_level
(
  log_level_t level,
  char * timestamp,
  char * p_filename,
  const char * p_function,
  uint32_t line_no,
  ...
);

#define LOG(level, ...) log_level(level,                 \
                                  NULL,                  \
                                  (char *)__FILE__,      \
                                  __FUNCTION__,          \
                                  __LINE__,              \
                                  __VA_ARGS__)

// Different log levels which can be turned on/off by setting LOG_LEVEL
#if LOG_LEVEL > 0
#define LOG_HIGH(...) LOG(LOG_LEVEL_HIGH, __VA_ARGS__)
#else
#define LOG_HIGH(...)
#endif /* LOG_LEVEL > 0 */

#if LOG_LEVEL > 1
#define LOG_MED(...)  LOG(LOG_LEVEL_MEDIUM, __VA_ARGS__)
#else
#define LOG_MED(...)
#endif  /* LOG_LEVEL > 1 */

#if LOG_LEVEL > 2
#define LOG_LOW(...)  LOG(LOG_LEVEL_LOW, __VA_ARGS__)
#else
#define LOG_LOW(...)
#endif  /* LOG_LEVEL > 2 */

#if LOG_LEVEL > 3
#define LOG_FUNC(...)  LOG(LOG_LEVEL_FUNC, __VA_ARGS__)
#else
#define LOG_FUNC(...)
#endif  /* LOG_LEVEL > 2 */

// Function entry log.  This should be placed at the beginning of each function
#define FUNC_ENTRY LOG_FUNC("Entering %s()", __FUNCTION__)

// Error and Fatal definitions.  Fatal should be used sparingly and
// mostly debugging
#define LOG_ERROR(...) LOG(LOG_LEVEL_ERROR, __VA_ARGS__)
#define LOG_FATAL(...) LOG(LOG_LEVEL_FATAL, __VA_ARGS__)

#endif /* __LOG_H__ */
