/** @file main_task.h
*
* @brief Main task interface
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _MAIN_TASK_H
#define _MAIN_TASK_H

#include "project_defs.h"

status_t init_main_task(int argc, char *argv[]);
status_t dest_main_task();
void main_task();

#endif /* _MAIN_TASK_H */
