/** @file air.h
*
* @brief Air task public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _AIR_H
#define _AIR_H

#include "project_defs.h"
#include "workers.h"

// Type of air reading
typedef enum air_meas_type
{
  AIR_TVOC,
  AIR_CO2
} air_meas_type_t;

typedef struct air_req
{
  uint8_t type;
  staleness_t staleness;
} air_req_t;

typedef struct air_rsp
{
  uint8_t type;
  uint16_t reading;
} air_rsp_t;

/*!
* @brief Initialize air task
* @return status of initializing air
*/
status_t init_air(uint8_t start_task);

/*!
* @brief Destroy air task
* @return status of destroying air
*/
status_t dest_air(uint8_t dest_task);

/*!
* @brief Sends a request for air quality
* @param type type either tvoc or co2
* @param staleness of reading (read new or keep old)
* @return status of sending request
*/
status_t send_air_req(air_meas_type_t type, staleness_t staleness, task_id_t from);
#endif /* _AIR_H */
