/** @file main.c
*
* @brief Main function
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "circbuf.h"
#include "log.h"

typedef struct test_struct {
  uint32_t id;
  uint8_t var1;
  uint16_t var2;
  uint32_t var3;
} test_struct_t;

void print(void * data, uint32_t index)
{
  test_struct_t * struct1 = (test_struct_t *)data;

  LOG_LOW("Data for index %d", index);
  LOG_LOW("id: %d, var1: %d, var2: %d, var3: %d",
          struct1->id,
          struct1->var1,
          struct1->var2,
          struct1->var3);
}

// Comparison function for ll_search
uint8_t compare(void * data1, void * data2)
{
  test_struct_t * struct1 = (test_struct_t *)data1;
  test_struct_t * struct2 = (test_struct_t *)data2;
  if (struct1->id == struct2->id)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void ll_test()
{
  FUNC_ENTRY;
  node_t * head;
  test_struct_t * input;
  test_struct_t * output;
  test_struct_t search;
  int32_t index;
  int32_t size;

  // Initialize linked list
  ll_init(&head);

  // Loop 10 times adding random data and inserting into ll
  for (uint16_t i = 0; i < 10; i++)
  {
    input = malloc(sizeof(*input));
    input->id = i;
    input->var1 = random();
    input->var2 = random();
    input->var3 = random();
    if (i == 4)
    {
      search = *input;
    }
    ll_insert(head, input, INSERT_AT_END);
  }

  // Search for the same item to make sure it was removed
  if (!ll_search(head, &search, compare, &index))
  {
    LOG_FATAL("Found node at index %d", index);
  }
  else
  {
    LOG_ERROR("Could not find node");
  }

  // Check size and print
  ll_size(head, &size);
  LOG_HIGH("size %d", size);

  // Remove an item and check size
  ll_remove(head, (void *)&output, index);
  ll_size(head, &size);
  LOG_HIGH("size %d", size);

  // Shod found and searched data
  LOG_HIGH("Searched Data:");
  print(&search, -1);
  LOG_HIGH("Found Data:");
  print(output, index);

  // Free the item removed
  free(output);

  // Search for the same item to make sure it was removed
  if (!ll_search(head, &search, compare, &index))
  {
    LOG_FATAL("Found node at index %d", index);
  }
  else
  {
    LOG_ERROR("Could not find node");
  }

  // Dump the linked list
  ll_dump(head, print);

  // Clean up
  ll_destroy(head);
}

void circbuf_test()
{
  FUNC_ENTRY;

  circbuf_t * buf;
  test_struct_t * input;
  const int buf_len = 10;
  const int extra = 3;

  // Init circbuf
  circbuf_init(&buf, buf_len);

  // Print the empty status
  LOG_FATAL("circbuf_empty: %d", circbuf_empty(buf));

  // Fill up circbuf
  for (uint32_t i = 0; i < buf_len; i++)
  {
    input = malloc(sizeof(*input));
    input->id = i;
    input->var1 = random();
    input->var2 = random();
    input->var3 = random();
    circbuf_add_item(buf, input);
  }

  // Dump contents
  circbuf_dump(buf, print);

  // Check full status
  LOG_FATAL("circbuf_full: %d", circbuf_full(buf));

  // Remove some items
  for (uint32_t i = 0; i < buf_len / 2; i++)
  {
    circbuf_remove_item(buf, (void **)&input);
    free(input);
  }

  for (uint32_t i = 0; i < extra; i++)
  {
    input = malloc(sizeof(*input));
    input->id = i + buf_len;
    input->var1 = random();
    input->var2 = random();
    input->var3 = random();
    circbuf_add_item(buf, input);
  }

  circbuf_dump(buf, print);

  for (uint32_t i = 0; i < buf_len / 2; i++)
  {
    circbuf_remove_item(buf, (void **)&input);
    free(input);
  }

  for (uint32_t i = 0; i < extra; i++)
  {
    input = malloc(sizeof(*input));
    input->id = i + buf_len + extra;
    input->var1 = random();
    input->var2 = random();
    input->var3 = random();
    circbuf_add_item(buf, input);
  }

  circbuf_dump(buf, print);

  circbuf_destroy_free(buf);
}

int main()
{
  FUNC_ENTRY;

  // Initialize log
  log_init();

  // Run a simple linked list test
  ll_test();

  // Run a simple circbuf test
  circbuf_test();

  // Destroy log
  log_destroy();
  return 0;
}
