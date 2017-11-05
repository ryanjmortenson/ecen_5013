/** @file unit_tests.c
*
* @brief Run unit tests
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "unit_circbuf.h"
#include "unit_light.h"
#include "unit_temp.h"
#include "unit_log_msg.h"
#include "unit_linkedlist.h"
#include "unit_main_task.h"

// Execute unit tests for log_msg.c
uint32_t unit_test_log_msg()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_log_msg_init),
    cmocka_unit_test(test_log_msg_send),
    cmocka_unit_test(test_log_msg_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for temp.c
uint32_t unit_test_temp()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_temp_init),
    cmocka_unit_test(test_temp_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for light.c
uint32_t unit_test_light()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_light_init),
    cmocka_unit_test(test_is_dark),
    cmocka_unit_test(test_light_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for main_task.c
uint32_t unit_test_main_task()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_main_init),
    cmocka_unit_test(test_main_send_hb),
    cmocka_unit_test(test_main_send_hb_setup),
    cmocka_unit_test(test_main_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for linkedlist.c
uint32_t unit_test_linkedlist()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_ll_init_destroy),
    cmocka_unit_test(test_ll_ops_null_ptr),
    cmocka_unit_test(test_ll_insert),
    cmocka_unit_test(test_ll_remove),
    cmocka_unit_test(test_ll_search),
    cmocka_unit_test(test_ll_size)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for circbuf.c
uint32_t unit_test_circbuf()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_circbuf_init_destroy),
    cmocka_unit_test(test_circbuf_ops_null_ptr),
    cmocka_unit_test(test_circbuf_null_buffer),
    cmocka_unit_test(test_circbuf_add_remove_full),
    cmocka_unit_test(test_circbuf_add_full),
    cmocka_unit_test(test_circbuf_remove_empty),
    cmocka_unit_test(test_circbuf_wrap_add),
    cmocka_unit_test(test_circbuf_wrap_remove),
    cmocka_unit_test(test_circbuf_check_full),
    cmocka_unit_test(test_circbuf_check_empty)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Main for unit tests
int main()
{
  unit_test_circbuf();
  unit_test_linkedlist();
  unit_test_main_task();
  unit_test_light();
  unit_test_temp();
  unit_test_log_msg();

  return 0;
}
