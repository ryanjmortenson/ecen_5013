/** @file client_task.h
*
* @brief Client task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _CLIENT_TASK_H
#define _CLIENT_TASK_H

#include "project_defs.h"

/*!
* @brief Initialize client task
* @return status of client
*/
status_t client_init();

/*!
* @brief Destroy client task
* @return status of destroying client
*/
status_t client_dest();

#endif // _CLIENT_TASK_H
