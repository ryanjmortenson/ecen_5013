/** @file unit_linkedlist.h
*
* @brief Declarations for unit linked list
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef __UNIT_LINKEDLIST_H__
#define __UNIT_LINKEDLIST_H__

/*
 * \brief test_ll_init_destroy: test ll_init ll_destroy under normal operations
 *
 */
void test_ll_init_destroy(void **state);

/*
 * \brief test_ll_ops_null_ptr: test ll operations handle null pointers gracefully
 *
 */
void test_ll_ops_null_ptr(void **state);

/*
 * \brief test_ll_insert: test insert functionality of linked list
 *
 */
void test_ll_insert(void **state);

/*
 * \brief test_ll_remove: test remove functionality of linked list
 *
 */
void test_ll_remove(void **state);

/*
 * \brief test_ll_search: test search functionality of linked list
 *
 */
void test_ll_search(void **state);

/*
 * \brief test_ll_size: test size functionality of linked list
 *
 */
void test_ll_size(void **state);

#endif // __UNIT_LINKEDLIST_H__
