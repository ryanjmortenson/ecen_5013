#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include "unit_circbuf.h"

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

  return 0;
}