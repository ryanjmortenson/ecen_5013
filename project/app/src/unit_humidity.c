/** @file unit_humidity.c
*
* @brief Unit tests for humidity.c
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

#include "humidity.h"
#include "workers.h"
#include "project_defs.h"

void test_humidity_init(void **state)
{
  assert_int_equal(init_humidity(1), FAILURE);
}

void test_humidity_dest(void **state)
{
  assert_int_equal(dest_humidity(1), FAILURE);
}
