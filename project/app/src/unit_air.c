/** @file unit_air.c
*
* @brief Unit tests for air.c
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

#include "air.h"
#include "workers.h"
#include "project_defs.h"

void test_air_init(void **state)
{
  assert_int_equal(init_air(1), FAILURE);
}

void test_air_dest(void **state)
{
  assert_int_equal(dest_air(1), FAILURE);
}
