/** @file unit_main_task.c
*
* @brief Unit tests for main task
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

#include "workers.h"
#include "main_task.h"

#define NUM_WORKERS (1)

char *good_argv[] = {
  "unit_test.out",
  "outuput.log"
};

char *bad_argv0[] = {
  "unit_test.out",
};

char *bad_argv1[] = {
  "unit_test.out",
  "output.log",
  "21"
};

char *bad_argv2[] = {
  "unit_test.out",
  "output.log",
  "-1"
};

void test_main_init(void **state)
{
  assert_int_equal(init_main_task(1, bad_argv0), FAILURE);
  assert_int_equal(init_main_task(3, bad_argv1), FAILURE);
  assert_int_equal(init_main_task(3, bad_argv2), FAILURE);
  assert_int_equal(init_main_task(2, good_argv), SUCCESS);
}

void test_main_send_hb(void **state)
{
  assert_int_equal(send_hb(TEST_TASK), SUCCESS);
}

void test_main_send_hb_setup(void **state)
{
  assert_int_equal(send_hb_setup(1, TEST_TASK), SUCCESS);
}

void test_main_dest(void **state)
{
  assert_int_equal(dest_main_task(), SUCCESS);
}
