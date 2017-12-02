/** @file project_defs.h
*
* @brief Holds common project definitions
*
*/

#ifndef __PROJECT_DEFS_H__
#define __PROJECT_DEFS_H__

// Read a new value or return stashed value
typedef enum staleness
{
  STALENESS_NEW,
  STALENESS_OLD
} staleness_t;

typedef enum status {
  SUCCESS,
  FAILURE
} status_t;

// Set pthread return types if tiva isn't being build
#ifndef TIVA
#define PTHREAD_RETURN_TYPE void *
#define PTHREAD_RETURN(val) pthread_exit(val)
#define gettimeofday(tv, extra)
#endif // TIVA

// Max file name
#define FILE_NAME_MAX (255)

// Helpful Macros
#define START_TIME start_timer(timer)
#define DISPLAY_TIMESTAMP LOG_FATAL("sec: %d, millisec: %f", \
                                    diff.tv_sec,             \
                                    (float)(diff.tv_nsec)/1000000)

#define GET_TIME stop_timer(timer);      \
                 get_time(timer, &diff);

// Null pointer check
#define CHECK_NULL(x) if (x == NULL) { return FAILURE; }
#define CHECK_NULL2(x) if (x == NULL) { LOG_ERROR("NULL pointer"); return NULL; }

// Check value not equal, print errno, and exit
#define NOT_EQ_EXIT_E(res, func, val)                           \
  if((res = func) != val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    exit(1);                                                    \
  }

// Check value not equal, print errno, and return failure
#define NOT_EQ_RET_E(res, func, val, ret)                       \
  if((res = func) != val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    return ret;                                                 \
  }

// Check value equal, print errno, and return failure
#define EQ_RET_E(res, func, val, ret)                           \
  if((res = func) == val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    return ret;                                                 \
  }

// Check value not equal, print errno, set abort, and return a value
#define NOT_EQ_RET_EA(res, func, val, ret, abort)               \
  if((res = func) != val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    abort = 1;                                                  \
    return ret;                                                 \
  }

// Check value equal, print errno, set abort, and return a value
#define EQ_RET_EA(res, func, val, ret, abort)                   \
  if((res = func) == val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    abort = 1;                                                  \
    return ret;                                                 \
  }

// Check value equal, print errno, and return failure
#define EQ_EXIT_E(res, func, val)                               \
  if((res = func) == val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(errno)); \
    exit(1);                                                    \
  }

#define PT_NOT_EQ_RET(res, func, val, ret)                      \
  if((res = func) != val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(res));   \
    return ret;                                                 \
  }

#define PT_EQ_RET(res, func, val, ret)                          \
  if((res = func) == val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(res));   \
    return ret;                                                 \
  }

#define PT_NOT_EQ_EXIT(res, func, val)                          \
  if((res = func) != val)                                       \
  {                                                             \
    LOG_ERROR(#func " failed with error: %s", strerror(res));   \
    exit(res);                                                  \
  }

#endif /* __PROJECT_DEFS_H__ */
