/** @file unit_apds9301.c
*
* @brief Unit tests for apds9301.c
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
#include "apds9301.h"

void test_apds9301_init(void **state)
{
  assert_int_equal(apds9301_init(2), SUCCESS);
}

void test_apds9301_dest(void **state)
{
  assert_int_equal(apds9301_dest(), SUCCESS);
}

void test_apds9301_w_cmd(void **state)
{
  command_reg_t cmd;
  assert_int_equal(apds9301_w_cmd(cmd), SUCCESS);
}

void test_apds9301_rw_byte(void **state)
{
  command_reg_t cmd;
  uint8_t byte;
  assert_int_equal(apds9301_r_byte(cmd, NULL), FAILURE);
  assert_int_equal(apds9301_r_byte(cmd, &byte), SUCCESS);
  assert_int_equal(apds9301_w_byte(cmd, byte), SUCCESS);
}

void test_apds9301_rw_word(void **state)
{
  command_reg_t cmd;
  uint8_t bytes[2];
  assert_int_equal(apds9301_r_word(cmd, NULL), FAILURE);
  assert_int_equal(apds9301_r_word(cmd, bytes), SUCCESS);
  assert_int_equal(apds9301_w_word(cmd, NULL), FAILURE);
  assert_int_equal(apds9301_w_word(cmd, bytes), SUCCESS);
}

void test_apds9301_r_lux(void **state)
{
  float lux;
  assert_int_equal(apds9301_r_lux(NULL), FAILURE);
  assert_int_equal(apds9301_r_lux(&lux), SUCCESS);
}

void test_apds9301_rw_ctrl(void **state)
{
  control_reg_t ctrl;
  assert_int_equal(apds9301_r_ctrl(NULL), FAILURE);
  assert_int_equal(apds9301_r_ctrl(&ctrl), SUCCESS);
  assert_int_equal(apds9301_w_ctrl(ctrl), SUCCESS);
}

void test_apds9301_w_timing(void **state)
{
  timing_reg_t timing;
  assert_int_equal(apds9301_w_timing(timing), SUCCESS);
}

void test_apds9301_w_inter_ctrl(void **state)
{
  interrupt_ctrl_reg_t intr;
  assert_int_equal(apds9301_w_inter_ctrl(intr), SUCCESS);
}

void test_apds9301_r_id(void **state)
{
  id_reg_t id;
  assert_int_equal(apds9301_r_id(NULL), FAILURE);
  assert_int_equal(apds9301_r_id(&id), SUCCESS);
}
