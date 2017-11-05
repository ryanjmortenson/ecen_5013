/** @file unit_temp.c
*
* @brief Unit tests for temp.c
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <mqueue.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "log.h"
#include "log_msg.h"
#include "project_defs.h"

#include <stdio.h>

#define TEST_LOG_NAME "test.log"

static const task_id_t TASK_ID = TEST_TASK;

void test_log_msg_init(void **state)
{
  struct stat fstat;
  assert_int_equal(log_msg_dest(), FAILURE);
  assert_int_equal(log_msg_init(NULL), FAILURE);
  assert_int_equal(log_msg_init(TEST_LOG_NAME), FAILURE);
  assert_int_equal(init_workers(1), SUCCESS);
  assert_int_equal(log_msg_init(TEST_LOG_NAME), SUCCESS);
  assert_int_equal(stat(TEST_LOG_NAME, &fstat), 0);
}

void test_log_msg_send(void **state)
{
  SEND_LOG_LOW("low");
  SEND_LOG_MED("med");
  SEND_LOG_HIGH("high");
  SEND_LOG_FUNC("func");
  SEND_LOG_ERROR("error");
  SEND_LOG_FATAL("fatal");
  SEND_INIT_COMPLETE();

  // TODO: Add verification of file
  usleep(1000000);
}

void test_log_msg_dest(void **state)
{
  assert_int_equal(log_msg_dest(), SUCCESS);
  assert_int_equal(log_msg_dest(), FAILURE);
  assert_int_equal(dest_workers(), SUCCESS);
  assert_int_equal(unlink(TEST_LOG_NAME), 0);
}
