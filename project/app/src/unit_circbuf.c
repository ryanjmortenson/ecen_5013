#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <cmocka.h>
#include "circbuf.h"
#include "project_defs.h"
#include "unit_circbuf.h"
#include "log.h"

#define BUF_SIZE (100)
#define HALF_BUF_SIZE (BUF_SIZE / 2)

// Result used in most tests
circbuf_t * buf = NULL;

void test_circbuf_init_destroy(void **state)
{
  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_init_destroy()

void test_circbuf_ops_null_ptr(void **state)
{
  uint8_t value = 0;
  uint8_t * p_value = &value;
  uint8_t insert = 1;

  // Pass a null pointer into each circbuf function and make sure they return
  // null pointer enum
  assert_int_equal(circbuf_init((circbuf_t **)NULL, BUF_SIZE), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_destroy((circbuf_t *)NULL), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_add_item((circbuf_t *)NULL, &insert), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_remove_item((circbuf_t *)NULL, (void **)&p_value), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_peek((circbuf_t *)NULL, 1, (void **)&p_value), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_empty((circbuf_t *)NULL), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_full((circbuf_t *)NULL), CB_ENUM_NULL_POINTER);
} // test_circbuf_init_null_ptr()

void test_circbuf_null_buffer(void **state)
{
  uint8_t value = 0;
  uint8_t * p_value = &value;
  uint8_t insert = 1;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Null out the buffer.  A function call needs to be made because the
  // circbuf struct is not exposed.
  assert_int_equal(circbuf_null_buffer(buf), CB_ENUM_NO_ERROR);

  // Check all operations return a null pointer
  assert_int_equal(circbuf_add_item(buf, (void *)&insert), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_NULL_POINTER);
  assert_int_equal(circbuf_peek(buf, 1, (void **)&p_value), CB_ENUM_NULL_POINTER);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NULL_POINTER);

  // Free the buffer because destroy will fail
  free(buf);

} // test_circbuf_init_null_ptr()

void test_circbuf_add_remove_full(void **state)
{
  uint8_t value[BUF_SIZE] = {0};
  uint8_t * p_value;

  // Create a circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Loop over buf size adding values checking there are no errors
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    value[i] = i;
    assert_int_equal(circbuf_add_item(buf, (void *)&value[i]), CB_ENUM_NO_ERROR);
  }

  // Loop over buf size removing values and checking contents and checking
  // there are no errors
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_NO_ERROR);
    assert_int_equal(*p_value, value[i]);
  }

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_add_remove_full()

void test_circbuf_add_full(void **state)
{
  uint8_t insert = 100;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Loop over buf size adding values checking there are no errors
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_add_item(buf, (void *)&i), CB_ENUM_NO_ERROR);
  }

  // Try to add an item when buffer is full to ensure it returns full
  assert_int_equal(circbuf_add_item(buf, (void *)&insert), CB_ENUM_FULL);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_add__full()

void test_circbuf_remove_empty(void **state)
{
  uint8_t value = 0;
  uint8_t * p_value = &value;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Try to remove an item from the empty buffer and check return for empty
  assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_EMPTY);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_add_remove_full()

void test_circbuf_wrap_add(void **state)
{
  uint8_t value[BUF_SIZE] = {0};
  uint8_t * p_value;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Loop over half of buf size adding values and checking for no error
  // This will place the head in the middle of the circular buffer
  for (uint32_t i = 0; i < HALF_BUF_SIZE; i++)
  {
    value[i] = i;
    assert_int_equal(circbuf_add_item(buf, (void *)&value[i]), CB_ENUM_NO_ERROR);
  }

  // Loop over half of buf size removing values checking for no error and
  // values are correct.
  // This will place the tail in the middle of the circular buffer
  for (uint32_t i = 0; i < HALF_BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_NO_ERROR);
    assert_int_equal(*p_value, value[i]);
  }

  // Loop over buf size adding values checking for no errors
  // This wraps the head back to the beginning
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    value[i] = i;
    assert_int_equal(circbuf_add_item(buf, &value[i]), CB_ENUM_NO_ERROR);
  }

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_wrap_add()

void test_circbuf_wrap_remove(void **state)
{
  uint8_t value1[HALF_BUF_SIZE] = {0};
  uint8_t value2[BUF_SIZE] = {0};
  uint8_t * p_value;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Loop over half of buf size adding values and checking for no error
  // This will place the head in the middle of the circular buffer
  for (uint32_t i = 0; i < HALF_BUF_SIZE; i++)
  {
    value1[i] = i;
    assert_int_equal(circbuf_add_item(buf, (void *)&value1[i]), CB_ENUM_NO_ERROR);
  }

  // Loop over half of buf size removing values checking for no error and
  // values are correct.
  // This will place the tail in the middle of the circular buffer
  for (uint32_t i = 0; i < HALF_BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_NO_ERROR);
    assert_int_equal(*p_value, value1[i]);
  }

  // Loop over buf size adding values checking for no errors
  // This wraps the head back to the beginning
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    value2[i] = i;
    assert_int_equal(circbuf_add_item(buf, (void *)&value2[i]), CB_ENUM_NO_ERROR);
  }

  // Loop over buf size removing values checking contents and checking no errors
  // This wraps the tail back to the beginning
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_remove_item(buf, (void **)&p_value), CB_ENUM_NO_ERROR);
    assert_int_equal(*p_value, value2[i]);
  }

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_wrap_remove()

void test_circbuf_check_full(void **state)
{
  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Check to make sure the full function returns a failure
  assert_int_equal(circbuf_full(buf), CB_ENUM_FAILURE);

  // Loop over buf size adding values checking for no errors
  for (uint32_t i = 0; i < BUF_SIZE; i++)
  {
    assert_int_equal(circbuf_add_item(buf, (void *)&i), CB_ENUM_NO_ERROR);
  }

  // Check to make sure the full function returns full
  assert_int_equal(circbuf_full(buf), CB_ENUM_FULL);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_check_full()

void test_circbuf_check_empty(void **state)
{
  uint8_t insert = 1;

  // Create circbuf and check there were no errors
  assert_int_equal(circbuf_init(&buf, BUF_SIZE), CB_ENUM_NO_ERROR);

  // Check for empty and make sure function returns empty because it is empty
  assert_int_equal(circbuf_empty(buf), CB_ENUM_EMPTY);

  // Add one item to make buffer not empty
  assert_int_equal(circbuf_add_item(buf, (void *)&insert), CB_ENUM_NO_ERROR);

  // Check for empty and make sure function returns failure because it isn't
  // empty
  assert_int_equal(circbuf_empty(buf), CB_ENUM_FAILURE);

  // Destroy circbuf and check there were no errors
  assert_int_equal(circbuf_destroy(buf), CB_ENUM_NO_ERROR);
} // test_circbuf_check_full()
