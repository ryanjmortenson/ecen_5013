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

#include "temp.h"
#include "project_defs.h"

void test_temp_init(void **state)
{
  assert_int_equal(init_temp(1), FAILURE);
}

void test_temp_dest(void **state)
{
  assert_int_equal(dest_temp(1), FAILURE);
}
