/** @file main_task.h
*
* @brief Main task interface
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _MAIN_TASK_H
#define _MAIN_TASK_H

#include "workers.h"
#include "project_defs.h"

#define SIGNAL_ABORT() abort_signal = 1

typedef struct hb_setup {
  uint32_t period_seconds;
} hb_setup_t;

/*!
* @brief Initialize main task
* @param[in] argc argument count
* @param[in] argv list of arguments
* @return status of setting up main task
*/
status_t init_main_task(int argc, char *argv[]);

/*!
* @brief Destroy main task
* @return status of destroying main task
*/
status_t dest_main_task();

/*!
* @brief Sends a heartbeat set up command
* @param[in] period_seconds how often the heartbeat will fire
* @param[in] from which task is registering
* @return status setting up hearbeat
*/
status_t send_hb_setup(uint32_t period_seconds, task_id_t from);

/*!
* @brief Sends a heartbeat
* @param[in] from which task is sending heartbeat
* @return status sending hearbeat
*/
status_t send_hb(task_id_t from);

/*!
* @brief Main task
*/
void main_task();
#endif /* _MAIN_TASK_H */
