/** @file unit_light.h
*
* @brief Unit tests for light task
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_LIGHT_H
#define _UNIT_LIGHT_H

/*!
* @brief Test init
*/
void test_light_init(void **state);

/*!
* @brief Test destroy
*/
void test_light_dest(void **state);

/*!
* @brief Test is dark
*/
void test_is_dark(void **state);

#endif /* _UNIT_LIGHT*/
