/** @file workers.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/
#include <mqueue.h>

#include "project_defs.h"

// Callback function typedef
typedef void * (*CALLBACK)(void * params);

// Types of messages.  Powers of 2 to use bitwise & to determine if the
// registration is for this type of message
typedef enum type {
  SHUTDOWN   = 0,
  LOG        = 1,
  TEMP_REQ   = 2,
  TEMP_RSP   = 4,
  LIGHT_REQ  = 8,
  LIGHT_RSP  = 16,
} type_t;

// Message structure
typedef struct message {
  type_t type;
  char msg[512];
} message_t;

/*!
* @brief Initialize worker threads
* @return status of initializing workers
*/
status_t init_workers(uint32_t num);

/*!
* @brief Destroy workers
* @return status of destroying workers
*/
status_t dest_workers();

/*!
* @brief Register a callback
* @param[in] type of messge to register for
* @param[in] callback used when message is received
* @return status of registering callback
*/
status_t register_cb(type_t type, CALLBACK cb);

/*!
* @brief Unregister a callback
* @param[in] type of messge to register for
* @param[in] callback used when message is received
* @return status of unregistering callback
*/
status_t unregister_cb(type_t type, CALLBACK cb);

/*!
* @brief flushes queue
* @return status of flushing queue
*/
status_t flush_queue();

/*!
* @brief gets a writeable queue
* @return queue descriptor
*/
mqd_t get_writeable_queue();
