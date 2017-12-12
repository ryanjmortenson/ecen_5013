/** @file server_task.h
*
* @brief Server task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _SERVER_TASK_H
#define _SERVER_TASK_H

/*!
* @brief Initialize server task
* @return status of server
*/
status_t server_init();

/*!
* @brief Destroy server task
* @return status of destroying server
*/
status_t server_dest();

/*!
* @brief Destroy a socket thread
*/
void clean_up_socket();

#endif // _SERVER_TASK_H
