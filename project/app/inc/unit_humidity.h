/** @file unit_humidity.h
*
* @brief Unit tests for humidity task
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_HUMIDITY_H
#define _UNIT_HUMIDITY_H

/*!
* @brief Test init
*/
void test_humidity_init(void **state);

/*!
* @brief Test destroy
*/
void test_humidity_dest(void **state);

#endif /* _UNIT_HUMIDITY*/
