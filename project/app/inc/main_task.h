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

status_t init_main_task(int argc, char *argv[]);
status_t dest_main_task();
status_t send_hb_setup(uint32_t period_seconds, task_id_t from);
status_t send_hb(task_id_t from);
void main_task();

#endif /* _MAIN_TASK_H */
