/** @file unit_apds9301.h
*
* @brief Unit tests apds9301
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_APDS9301_H
#define _UNIT_APDS9301_H

/*!
* @brief Test init
*/
void test_apds9301_init(void **state);

/*!
* @brief Test destroy
*/
void test_apds9301_dest(void **state);

/*!
* @brief Test writing of command register
*/
void test_apds9301_w_cmd(void **state);

/*!
* @brief Test read/write of a byte
*/
void test_apds9301_rw_byte(void **state);

/*!
* @brief Test read/write of a word
*/
void test_apds9301_rw_word(void **state);

/*!
* @brief Test read of lux
*/
void test_apds9301_r_lux(void **state);

/*!
* @brief Test read/write of ctrl register
*/
void test_apds9301_rw_ctrl(void **state);

/*!
* @brief Test read/write timing register
*/
void test_apds9301_w_timing(void **state);

/*!
* @brief Test write of interlock control register
*/
void test_apds9301_w_inter_ctrl(void **state);

/*!
* @brief Test read of id
*/
void test_apds9301_r_id(void **state);

#endif /* _UNIT_TMP102_H*/
