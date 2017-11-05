/** @file unit_apds9301.h
*
* @brief Unit tests apds9301
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_APDS9301_H
#define _UNIT_APDS9301_H

void test_apds9301_init(void **state);
void test_apds9301_dest(void **state);
void test_apds9301_w_cmd(void **state);
void test_apds9301_rw_byte(void **state);
void test_apds9301_rw_word(void **state);
void test_apds9301_r_lux(void **state);
void test_apds9301_rw_ctrl(void **state);
void test_apds9301_w_timing(void **state);
void test_apds9301_w_inter_ctrl(void **state);
void test_apds9301_r_id(void **state);

#endif /* _UNIT_TMP102_H*/
