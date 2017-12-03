/** @file main_server_task.h
*
* @brief Main task interface
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _MAIN_SERVER_TASK_H
#define _MAIN_SERVER_TASK_H

#include "workers.h"
#include "project_defs.h"

/*!
* @brief Initialize main task
* @param[in] argc argument count
* @param[in] argv list of arguments
* @return status of setting up main task
*/
status_t init_main_server_task(int argc, char *argv[]);

/*!
* @brief Destroy main task
* @return status of destroying main task
*/
status_t dest_main_server_task();

/*!
* @brief Main task
*/
void main_server_task();

status_t initialize_q();
#endif /* _MAIN_SERVER_TASK_H */
