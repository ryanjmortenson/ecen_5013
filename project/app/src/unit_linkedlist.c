/** @file unit_linkedlist.c
*
* @brief Unit tests for linked list
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include "cmocka.h"
#include "linkedlist.h"
#include "log.h"
#include "project_defs.h"
#include "unit_linkedlist.h"

#define LIST_SIZE (100)
#define HALF_LIST_SIZE (LIST_SIZE / 2)

// Test structure for inserting and removing
typedef struct test_data {
  uint8_t data1;
  uint16_t data2;
  uint32_t data3;
} test_data_t;

/*
 * \brief insert_data: Insert random data into a test structure
 *
 * \param data: pointer to the data structure
 * \return: success or error
 *
 */
void insert_data(test_data_t * data)
{
  data->data1 = random();
  data->data2 = random();
  data->data3 = random();
}

/*
 * \brief compare: Function to compare two data structures
 *
 * \param data1: pointer to the first data structure
 * \param data2: pointer to the second data structure
 * \return: 1 is a match 0 is not a match
 *
 */
uint8_t compare(void * data1, void * data2)
{
  test_data_t * casted_data1 = (test_data_t *) data1;
  test_data_t * casted_data2 = (test_data_t *) data2;

  if (casted_data1->data1 == casted_data2->data1 &&
      casted_data1->data2 == casted_data2->data2 &&
      casted_data1->data3 == casted_data2->data3)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void test_ll_init_destroy(void **state)
{
  node_t * p_head = NULL;

  // Create ll and check there were no errors
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Destroy ll and check there were no errors
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_init_destroy()

void test_ll_ops_null_ptr(void **state)
{
  int32_t index = 1;
  int32_t data = 0;
  int32_t size = 0;
  int32_t * p_data = &data;
  node_t * p_head = NULL;

  // Initialize a good head to test other parameters having NULL pointers
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Test init/destroy
  assert_int_equal(ll_init((node_t **)NULL), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_destroy((node_t *)NULL), LL_ENUM_NULL_POINTER);

  // Test insert with null pointers
  assert_int_equal(ll_insert((node_t *)NULL, &data, index), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_insert(p_head, NULL, index), LL_ENUM_NULL_POINTER);

  // Test remove with null pointers
  assert_int_equal(ll_remove((node_t *)NULL, (void *)&p_data, index), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_remove(p_head, NULL, index), LL_ENUM_NULL_POINTER);

  // Test search with null pointers
  assert_int_equal(ll_search((node_t *)NULL, (void *)&data, compare, &index), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_search(p_head, NULL, compare, &index), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_search(p_head, (void *)&data, NULL, &index), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_search(p_head, (void *)&data, NULL, NULL), LL_ENUM_NULL_POINTER);

  // Test size with null pointers
  assert_int_equal(ll_size((node_t *)NULL, &size), LL_ENUM_NULL_POINTER);
  assert_int_equal(ll_size(p_head, NULL), LL_ENUM_NULL_POINTER);

  // Destroy the good head
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_ops_null_ptr()

void test_ll_insert(void **state)
{
  srandom(time(NULL));
  node_t * p_head = NULL;
  uint32_t num_inserts = random() % HALF_LIST_SIZE;
  test_data_t * data = NULL;

  // Make sure the random number of inserts is > 0
  while (num_inserts == 0)
  {
    num_inserts = random() % HALF_LIST_SIZE;
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Insert a random number of nodes into list
  for (uint32_t i = 0; i < num_inserts; i++)
  {
    data = malloc(sizeof(*data));
    insert_data(data);
    assert_int_equal(ll_insert(p_head, data, INSERT_AT_END), LL_ENUM_NO_ERROR);
  }

  // Insert a node at an index that isn't possible
  assert_int_equal(ll_insert(p_head, data, num_inserts + 1), LL_ENUM_INDEX_TOO_LARGE);

  // Insert an item in the middle
  data = malloc(sizeof(*data));
  insert_data(data);
  assert_int_equal(ll_insert(p_head, data, num_inserts / 2), LL_ENUM_NO_ERROR);

  // Insert an item in the beginning
  data = malloc(sizeof(*data));
  insert_data(data);
  assert_int_equal(ll_insert(p_head, data, 0), LL_ENUM_NO_ERROR);

  // Destroy ll and check there were no errors
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_insert()

void test_ll_remove(void **state)
{
  node_t * p_head = NULL;
  uint32_t num_inserts = 0;
  test_data_t * p_data = NULL;
  test_data_t * p_removed_data = NULL;
  test_data_t * p_zeroith_data = NULL;
  test_data_t * p_middle_data = NULL;

  // Make sure the random number of inserts is > 0
  while (num_inserts < 3)
  {
    num_inserts = random() % HALF_LIST_SIZE;
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Try to remove from empty list
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, 0), LL_ENUM_INDEX_TOO_LARGE);

  // Insert a random number of nodes into list
  for (uint32_t i = 0; i < num_inserts; i++)
  {
    p_data = malloc(sizeof(*p_data));
    insert_data(p_data);
    if (i == 0)
    {
      p_zeroith_data = p_data;
    }
    if (i == num_inserts / 2)
    {
      p_middle_data = p_data;
    }
    assert_int_equal(ll_insert(p_head, p_data, INSERT_AT_END), LL_ENUM_NO_ERROR);
  }

  // Remove the middle item
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, num_inserts / 2), LL_ENUM_NO_ERROR);
  assert_ptr_equal(p_middle_data, p_removed_data);
  free(p_removed_data);

  // Remove the 0th item
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, 0), LL_ENUM_NO_ERROR);
  assert_ptr_equal(p_zeroith_data, p_removed_data);
  free(p_removed_data);

  // Remove from end of list
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, REMOVE_AT_END), LL_ENUM_NO_ERROR);
  assert_ptr_equal(p_data, p_removed_data);
  free(p_removed_data);

  // Try to an index that is out of range
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, num_inserts), LL_ENUM_INDEX_TOO_LARGE);

  // Destroy ll and check there were no errors
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_remove()

void test_ll_search(void **state)
{
  node_t * p_head = NULL;
  uint32_t num_inserts = 0;
  int32_t index = 0;
  test_data_t * p_data = NULL;
  test_data_t * p_removed_data = NULL;
  test_data_t * p_zeroith_data = NULL;
  test_data_t * p_middle_data = NULL;

  // Make sure the random number of inserts is > 0
  while (num_inserts == 0)
  {
    num_inserts = random() % HALF_LIST_SIZE;
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Try to remove from empty list
  assert_int_equal(ll_remove(p_head, (void **)&p_removed_data, 0), LL_ENUM_INDEX_TOO_LARGE);

  // Insert a random number of nodes into list
  for (uint32_t i = 0; i < num_inserts; i++)
  {
    p_data = malloc(sizeof(*p_data));
    insert_data(p_data);
    if (i == 0)
    {
      p_zeroith_data = p_data;
    }
    if (i == num_inserts / 2)
    {
      p_middle_data = p_data;
    }
    assert_int_equal(ll_insert(p_head, p_data, INSERT_AT_END), LL_ENUM_NO_ERROR);
  }

  // Search and remove the middle item
  assert_int_equal(ll_search(p_head, (void *)p_middle_data, compare, &index), LL_ENUM_NO_ERROR);
  assert_int_equal(index, num_inserts / 2);

  // Search and remove the 0th item
  assert_int_equal(ll_search(p_head, (void *)p_zeroith_data, compare, &index), LL_ENUM_NO_ERROR);
  assert_int_equal(index, 0);

  // Destroy ll and check there were no errors
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_search()

void test_ll_size(void **state)
{
  node_t * p_head = NULL;
  uint32_t num_inserts = random() % HALF_LIST_SIZE;
  uint32_t num_removes = random() % num_inserts;
  int32_t size = 0;
  test_data_t * p_data = NULL;

  // Make sure the random number of inserts is > 0
  while (num_inserts == 0)
  {
    num_inserts = random() % HALF_LIST_SIZE;
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_init(&p_head), LL_ENUM_NO_ERROR);

  // Create a ll and check there were no errors
  assert_int_equal(ll_size(p_head, &size), LL_ENUM_NO_ERROR);
  assert_int_equal(size, 0);

  // Insert a random number of nodes into list
  for (uint32_t i = 0; i < num_inserts; i++)
  {
    p_data = malloc(sizeof(*p_data));
    insert_data(p_data);
    assert_int_equal(ll_insert(p_head, p_data, INSERT_AT_END), LL_ENUM_NO_ERROR);
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_size(p_head, &size), LL_ENUM_NO_ERROR);
  assert_int_equal(size, num_inserts);

  // Insert a random number of nodes into list
  for (uint32_t i = 0; i < num_removes; i++)
  {
    assert_int_equal(ll_remove(p_head, (void **)&p_data, REMOVE_AT_END), LL_ENUM_NO_ERROR);
    free(p_data);
  }

  // Create a ll and check there were no errors
  assert_int_equal(ll_size(p_head, &size), LL_ENUM_NO_ERROR);
  assert_int_equal(size, num_inserts - num_removes);

  // Destroy ll and check there were no errors
  assert_int_equal(ll_destroy(p_head), LL_ENUM_NO_ERROR);
} // test_ll_size()
