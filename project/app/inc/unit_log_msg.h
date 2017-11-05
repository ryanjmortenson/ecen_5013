/** @file unit_log_msg.h
*
* @brief Unit tests for log_msg task
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_LOG_MSG_H
#define _UNIT_LOG_MSG_H

void test_log_msg_init(void **state);
void test_log_msg_send(void **state);
void test_log_msg_dest(void **state);

#endif /* _UNIT_LOG_MSG*/
