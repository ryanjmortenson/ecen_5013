/** @file temp.h
*
* @brief Worker public API
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _TEMP_H
#define _TEMP_H

#include "workers.h"

// What the reading should be in
typedef enum temp_units
{
  TEMP_UNITS_C,
  TEMP_UNITS_K,
  TEMP_UNITS_F
} temp_units_t;

typedef struct temp_req
{
  temp_units_t temp_units;
  staleness_t staleness;
} temp_req_t;

typedef struct temp_rsp
{
  temp_units_t temp_units;
  float temp;
} temp_rsp_t;

/*!
* @brief Initialize temperature task
* @return status of initializing temp
*/
status_t init_temp(uint8_t start_task);

/*!
* @brief Destroy temperature task
* @return status of destroying temp
*/
status_t dest_temp(uint8_t dest_task);

/*!
* @brief Sends a request for temperature
* @param temp_units units for the temp to returned in
* @param staleness of reading (read new or keep old)
* @return status of destroying temp
*/
status_t send_temp_req
(
  temp_units_t temp_units,
  staleness_t staleness,
  task_id_t from
);

// Macros for getting temp with different units
#define get_temp_f(staleness, from) send_temp_req(TEMP_UNITS_F, staleness, from)
#define get_temp_c(staleness, from) send_temp_req(TEMP_UNITS_C, staleness, from)
#define get_temp_k(staleness, from) send_temp_req(TEMP_UNITS_K, staleness, from)
#endif /* _TEMP_H */
