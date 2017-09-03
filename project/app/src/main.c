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
#include "log.h"

uint8_t compare(void * data1, void * data2)
{
  FUNC_ENTRY;
  if (*(uint8_t *)data1 == *(uint8_t *)data2)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int main()
{
  FUNC_ENTRY;
  node_t * head;
  int32_t index;

  log_init();
  ll_init(&head);
  uint8_t * test;
  uint8_t test1;

  for (uint16_t i = 0; i < 10; i++)
  {
    test = malloc(sizeof(*test));
    *test = i;
    ll_insert(head, test, INSERT_AT_END);
  }

  test = malloc(sizeof(*test));
  test1 = 4;
  ll_search(head, &test1, compare, &index);
  ll_size(head, &index);
  LOG_HIGH("size %d", index);
  ll_remove(head, (void *)&test, 4);
  ll_size(head, &index);
  LOG_HIGH("size %d", index);
  LOG_HIGH("*test %d", *test);
  free(test);
  test1 = 4;
  ll_search(head, &test1, compare, &index);
  ll_destroy(head);
  log_destroy();
  return 0;
}
