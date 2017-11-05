/** @file unit_tmp102.c
*
* @brief Unit tests for tmp102.c
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

#include "project_defs.h"
#include "tmp102.h"

int temps[][3] = {
  {0x7ff, 128, 127},
  {0x640, 101, 99},
  {0x500, 81, 79},
  {0x4B0, 76, 74},
  {0x320, 51, 49},
  {0x190, 26, 24},
  {0x004, 1, 0},
  {0x000, 1, -1},
  {0xffc, 0, -1},
  {0xe70, -24, -26},
  {0xc90, -54, -56}
};

void test_tmp102_conv(void **state)
{
  float conversion;

  for (uint8_t i = 0; i < 11; i++)
  {
    conversion = convert_temp(temps[i][0]);
    assert_int_equal((conversion < temps[i][1] && conversion > temps[i][2]), 1);
  }
}

void test_tmp102_init(void **state)
{
  assert_int_equal(tmp102_init(2), SUCCESS);
}

void test_tmp102_dest(void **state)
{
  assert_int_equal(tmp102_dest(), SUCCESS);
}

void test_tmp102_w_ptr(void **state)
{
  pointer_reg_t reg = POINTER_CONFIG;
  assert_int_equal(tmp102_w_ptr(reg), SUCCESS);
}

void test_tmp102_rw_reg(void **state)
{
  pointer_reg_t reg = POINTER_CONFIG;
  uint8_t bytes[2];
  assert_int_equal(tmp102_r_reg(reg, NULL), FAILURE);
  assert_int_equal(tmp102_r_reg(reg, bytes), SUCCESS);
  assert_int_equal(tmp102_w_reg(reg, NULL), FAILURE);
  assert_int_equal(tmp102_w_reg(reg, bytes), SUCCESS);
}

void test_tmp102_r_tmp(void **state)
{
  float temp;
  assert_int_equal(tmp102_r_tmp(NULL), FAILURE);
  assert_int_equal(tmp102_r_tmp(&temp), SUCCESS);
}

void test_tmp102_rw_cfg(void **state)
{
  config_reg_t cfg;
  assert_int_equal(tmp102_r_cfg(NULL), FAILURE);
  assert_int_equal(tmp102_r_cfg(&cfg), SUCCESS);
  assert_int_equal(tmp102_w_cfg(cfg), SUCCESS);
}

void test_tmp102_sd_mode(void **state)
{
  assert_int_equal(tmp102_sd_mode(3), SUCCESS);
}

void test_tmp102_set_cr(void **state)
{
  assert_int_equal(tmp102_set_cr(3), SUCCESS);
}
