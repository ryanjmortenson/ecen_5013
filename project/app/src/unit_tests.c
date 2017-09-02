#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include "unit_data.h"
#include "unit_circbuf.h"
#include "unit_memory.h"
#include "unit_tests.h"

// Execute unit tests for memory.c
uint32_t unit_test_memory()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_my_memmove),
    cmocka_unit_test(test_my_memmove_dst_starts_in_src),
    cmocka_unit_test(test_my_memmove_dst_ends_in_src),
    cmocka_unit_test(test_my_memmove_null_ptrs),
    cmocka_unit_test(test_my_memset),
    cmocka_unit_test(test_my_memset_null_ptr),
    cmocka_unit_test(test_my_memzero),
    cmocka_unit_test(test_my_memzero_null_ptr),
    cmocka_unit_test(test_my_reverse_odd),
    cmocka_unit_test(test_my_reverse_even),
    cmocka_unit_test(test_my_reverse_single),
    cmocka_unit_test(test_my_reverse_max),
    cmocka_unit_test(test_my_reverse_null_ptr),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for data.c
uint32_t unit_test_data()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_big_to_little32),
    cmocka_unit_test(test_big_to_little32_null_ptr),
    cmocka_unit_test(test_little_to_big32),
    cmocka_unit_test(test_little_to_big32_null_ptr)
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
  unit_test_memory();
  unit_test_data();
  unit_test_circbuf();

  return 0;
}
