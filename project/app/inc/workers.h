/** @file workers.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _WORKERS_H
#define _WORKERS_H

#ifndef TIVA
#include <mqueue.h>
#else
#include "mqueue_wrapper.h"
#endif // TIVA

#include "project_defs.h"

// Callback function typedef
typedef void * (*CALLBACK)(void * params);

// Types of messages.  Powers of 2 to use bitwise & to determine if the
// registration is for this type of message
typedef enum type {
  SHUTDOWN   = 0,
  LOG,
  AIR_REQ,
  AIR_RSP,
  TEMP_REQ,
  TEMP_RSP,
  LIGHT_REQ,
  LIGHT_RSP,
  IS_DARK_REQ,
  IS_DARK_RSP,
  HUMIDITY_REQ,
  HUMIDITY_RSP,
  HEARTBEAT_SETUP,
  HEARTBEAT,
  UNROUTED
} type_t;

typedef enum task_id {
  ALL_TASKS,
  MAIN_TASK,
  LIGHT_TASK,
  TEMP_TASK,
  LOG_TASK,
  AIR_TASK,
  CLIENT_TASK,
  SERVER_TASK,
  HUMIDITY_TASK,
  TEST_TASK,
  TASK_ID_LIST_END
} task_id_t;

// Message structure
typedef struct message {
  int32_t fd;
  uint8_t type;
  uint8_t network_routed;
  uint8_t to;
  uint8_t from;
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
status_t register_cb(type_t type, task_id_t to, CALLBACK cb);

/*!
* @brief Unregister a callback
* @param[in] type of messge to register for
* @param[in] callback used when message is received
* @return status of unregistering callback
*/
status_t unregister_cb(type_t type, task_id_t to, CALLBACK cb);

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

/*!
* @brief Sends a message
* @param[in] msg_q message queue descriptor for send
* @param[in] type type of message to send
* @param[in] data data to send
* @param[in] len length of data to send
* @return Status of send
*/
status_t send_msg(mqd_t msg_q, message_t * msg, void * data, uint32_t len);
#define MSG_INIT(_type, _to, _from) {.type = _type, .to = _to, .from = _from, .network_routed = 0}

#endif // _WORKERS_H
