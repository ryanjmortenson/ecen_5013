/** @file unit_tmp102.h
*
* @brief Unit tests tmp102
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_TMP102_H
#define _UNIT_TMP102_H

/*!
* @brief Test temperature conversion
*/
void test_tmp102_conv(void **state);

/*!
* @brief Test init
*/
void test_tmp102_init(void **state);

/*!
* @brief Test destroy
*/
void test_tmp102_dest(void **state);

/*!
* @brief Test write of pointer register
*/
void test_tmp102_w_ptr(void **state);

/*!
* @brief Test read/write of any register
*/
void test_tmp102_rw_reg(void **state);

/*!
* @brief Test read of temp
*/
void test_tmp102_r_tmp(void **state);

/*!
* @brief Test read/write of config
*/
void test_tmp102_rw_cfg(void **state);

/*!
* @brief Test setting of sd mode
*/
void test_tmp102_sd_mode(void **state);

/*!
* @brief Test setting conversion resolution
*/
void test_tmp102_set_cr(void **state);

#endif /* _UNIT_TMP102_H*/
