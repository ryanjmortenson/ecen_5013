/** @file unit_main_task.h
*
* @brief Unit tests for main task
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_MAIN_TASK_H
#define _UNIT_MAIN_TASK_H

void test_main_init(void **state);
void test_main_dest(void **state);
void test_main_send_hb(void **state);
void test_main_send_hb_setup(void **state);

#endif /* _UNIT_MAIN_TASK_H */
