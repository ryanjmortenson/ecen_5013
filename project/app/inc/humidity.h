/** @file humidity.h
*
* @brief Air task public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _HUMIDITY_H
#define _HUMIDITY_H

#include "project_defs.h"
#include "workers.h"

typedef struct humidity_req
{
  staleness_t staleness;
} humidity_req_t;

typedef struct humidity_rsp
{
  uint16_t humidity;
} humidity_rsp_t;
/*!
* @brief Initialize humidity task
* @return status of initializing humidity
*/
status_t init_humidity(uint8_t start_task);

/*!
* @brief Destroy humidity task
* @return status of destroying humidity
*/
status_t dest_humidity(uint8_t dest_task);

/*!
* @brief Sends a request for humidity
* @param staleness of reading (read new or keep old)
* @return status of sending request
*/
status_t send_humidity_req(staleness_t staleness, task_id_t from);

#endif /* _HUMIDITY_H */
