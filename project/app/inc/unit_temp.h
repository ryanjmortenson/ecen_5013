/** @file unit_temp.h
*
* @brief Unit tests for temp task
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _UNIT_TEMP_H
#define _UNIT_TEMP_H

void test_temp_init(void **state);
void test_temp_dest(void **state);
void test_is_dark(void **state);

#endif /* _UNIT_TEMP*/
