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

#include "unit_apds9301.h"
#include "unit_circbuf.h"
#include "unit_light.h"
#include "unit_temp.h"
#include "unit_air.h"
#include "unit_humidity.h"
#include "unit_tmp102.h"
#include "unit_log_msg.h"
#include "unit_linkedlist.h"
#include "unit_main_task.h"

// Execute unit tests for apds9301
uint32_t unit_test_apds9301()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_apds9301_init),
    cmocka_unit_test(test_apds9301_w_cmd),
    cmocka_unit_test(test_apds9301_rw_byte),
    cmocka_unit_test(test_apds9301_rw_word),
    cmocka_unit_test(test_apds9301_r_lux),
    cmocka_unit_test(test_apds9301_rw_ctrl),
    cmocka_unit_test(test_apds9301_w_timing),
    cmocka_unit_test(test_apds9301_w_inter_ctrl),
    cmocka_unit_test(test_apds9301_r_id),
    cmocka_unit_test(test_apds9301_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for tmp102
uint32_t unit_test_tmp102()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_tmp102_conv),
    cmocka_unit_test(test_tmp102_init),
    cmocka_unit_test(test_tmp102_w_ptr),
    cmocka_unit_test(test_tmp102_rw_reg),
    cmocka_unit_test(test_tmp102_r_tmp),
    cmocka_unit_test(test_tmp102_rw_cfg),
    cmocka_unit_test(test_tmp102_sd_mode),
    cmocka_unit_test(test_tmp102_set_cr),
    cmocka_unit_test(test_tmp102_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

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

// Execute unit tests for humidity.c
uint32_t unit_test_humidity()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_humidity_init),
    cmocka_unit_test(test_humidity_dest)
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}

// Execute unit tests for air.c
uint32_t unit_test_air()
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_air_init),
    cmocka_unit_test(test_air_dest)
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
  unit_test_humidity();
  unit_test_air();
  unit_test_log_msg();
  unit_test_tmp102();
  unit_test_apds9301();
  return 0;
}
