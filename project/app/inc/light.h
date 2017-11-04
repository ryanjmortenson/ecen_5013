/** @file light.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _LIGHT_H
#define _LIGHT_H

#include "project_defs.h"
#include "workers.h"

typedef struct light_req
{
  staleness_t staleness;
} light_req_t;

typedef struct light_rsp
{
  float lux;
} light_rsp_t;

/*!
* @brief Initialize light task
* @return status of initializing light
*/
status_t init_light();

/*!
* @brief Destroy light task
* @return status of destroying light
*/
status_t dest_light();

/*!
* @brief Determine if it is dark
* @param dark darkness indicator
* @return status of making call
*/
status_t is_dark(uint8_t * dark);

/*!
* @brief Sends a request for light
* @param light_units units for the light to returned in
* @param staleness of reading (read new or keep old)
* @return status of destroying light
*/
status_t send_light_req(staleness_t staleness, task_id_t from);

#endif /* _LIGHT_H */
