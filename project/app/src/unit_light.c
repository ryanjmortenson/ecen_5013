/** @file unit_light.c
*
* @brief Unit tests for light.c
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

#include "light.h"
#include "workers.h"
#include "project_defs.h"

void test_light_init(void **state)
{
  assert_int_equal(init_light(), FAILURE);
}

void test_is_dark(void **state)
{
  uint8_t dark;
  assert_int_equal(is_dark(NULL), FAILURE);
  assert_int_equal(is_dark(&dark), SUCCESS);
}

void test_light_dest(void **state)
{
  assert_int_equal(dest_light(), FAILURE);
}
