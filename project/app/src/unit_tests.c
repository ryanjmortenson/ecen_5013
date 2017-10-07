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
#include "unit_linkedlist.h"

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

  return 0;
}
