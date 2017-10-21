/** @file log.c
*
* @brief Logging functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#if defined(SYS_LOG) && defined(COLOR_LOGS)
  #error "Color logs not allowed in system log"
#endif

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/time.h>

#include "log.h"
#include "project_defs.h"

#define LOG_BUFFER_MAX (1024)
#define STRNCAT_MAX (LOG_BUFFER_MAX - 1)

// String log levels
static const char * p_log_level_str[] = {
  "HIGH",
  "MEDIUM",
  "LOW",
  "FUNC",
  "ERROR",
  "FATAL"
};

#ifdef COLOR_LOGS
// String log level colors
static const char * p_log_color_str[] = {
  "\e[1;96m", // High=CYAN
  "\e[1;93m", // Medium=YELLOW
  "\e[1;97m", // Low=WHITE
  "\e[1;30m", // Func=GRAY
  "\e[1;91m", // Error=RED
  "\e[1;95m", // Fatal=PURPLE
};

// Format for color
#define LOG_COLOR_FMT   "%s%-10s %-7s %-10.10s [%10.10s] %4u: "

#else
// Format for non-color logs
#define LOG_FMT         "%-10s %-7s %-10.10s [%10.10s] %4u: "

#endif /* COLOR_LOGS */

const char * get_log_level_string(log_level_t level)
{
  return p_log_level_str[level];
}

static inline status_t get_time_stamp(char * buf)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  create_timestamp(&tv, buf);
  return SUCCESS;
}

void create_timestamp(struct timeval * tv, char * buf)
{
  snprintf(buf, TIMESTAMP_LEN, "%f", (double) (tv->tv_sec + ((double) tv->tv_usec / 1000000)));
}

/*!
* @brief Get the file basename in a OS that uses "/" for the separator
* @param[in] p_filename pointer to the file name
* @param[in] p_path_separator pointer to the path separator
* @return pointer to the basename
*/
char * get_basename(char * p_filename, const char * p_path_seperator)
{
  uint8_t found_last_occurence = 0;

  // Loop of string looking for instances of p_path_separator until
  // the last instance is found.  Use that as basename.
  while (!found_last_occurence)
  {
    char * p_temp = strstr(p_filename, p_path_seperator);
    if (p_temp != NULL)
    {
      p_filename = p_temp + 1;
    }
    else
    {
      found_last_occurence = 1;
    }
  }

  // Return a pointer to the file name
  return p_filename;
} // get_basename()

void log_init()
{
#ifdef SYS_LOG
  openlog("ecen5623", LOG_CONS | LOG_PID, LOG_USER);
#endif
} // log_init()

void log_destroy()
{
#ifdef SYS_LOG
  closelog();
#endif
} // log_destroy()

void log_level
(
  log_level_t level,
  char * timestamp,
  char * p_filename,
  const char * p_function,
  uint32_t line_no,
  ...
)
{
  va_list printf_args;
  char * fmt;
  char log_buffer[LOG_BUFFER_MAX];
  char fmt_buffer[LOG_BUFFER_MAX];
  char ts[TIMESTAMP_LEN];

  // Point to the last argument where the variadic arguments start
  va_start(printf_args, line_no);

  // Get the first argument which will be the format for the printf statement
  fmt = va_arg(printf_args, char *);

  if (timestamp == NULL)
  {
    get_time_stamp(ts);
    timestamp = ts;
  }

#ifdef COLOR_LOGS
  // Print header in color
  snprintf(log_buffer,
           LOG_BUFFER_MAX,
           LOG_COLOR_FMT,
           p_log_color_str[level],
           timestamp,
           p_log_level_str[level],
           get_basename(p_filename, PATH_SEPARATOR),
           p_function,
           line_no);
#else
  // Print the header without color
  snprintf(log_buffer,
           LOG_BUFFER_MAX,
           LOG_FMT,
           timestamp,
           p_log_level_str[level],
           get_basename(p_filename, PATH_SEPARATOR),
           p_function,
           line_no);

#endif /* COLOR_LOGS */
  // Print the statement provided in the ... variadic parameter
  vsnprintf(fmt_buffer, LOG_BUFFER_MAX, fmt, printf_args);
  strncat(log_buffer, fmt_buffer, STRNCAT_MAX);

#ifdef COLOR_LOGS
  // Print the ending color and newline
  strncat(log_buffer, "\e[0m\n", STRNCAT_MAX);
#else
  //printf("\n");
  strncat(log_buffer, "\n", STRNCAT_MAX);
#endif /* COLOR_LOGS */
  va_end(printf_args);

#ifdef SYS_LOG
  // Log to syslog
  syslog(LOG_INFO, "%s", log_buffer);
#else
  // Print the generated string
  printf("%s", log_buffer);
#endif // SYSLOG

} // log_level()
