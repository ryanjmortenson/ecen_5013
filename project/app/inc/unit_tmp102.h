/** @file unit_tmp102.h
*
* @brief Unit tests tmp102
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_TMP102_H
#define _UNIT_TMP102_H

void test_tmp102_conv(void **state);
void test_tmp102_init(void **state);
void test_tmp102_dest(void **state);
void test_tmp102_w_ptr(void **state);
void test_tmp102_rw_reg(void **state);
void test_tmp102_r_tmp(void **state);
void test_tmp102_rw_cfg(void **state);
void test_tmp102_sd_mode(void **state);
void test_tmp102_set_cr(void **state);

#endif /* _UNIT_TMP102_H*/
