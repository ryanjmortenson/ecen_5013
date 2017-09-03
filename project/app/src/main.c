#include <stdint.h>
#include <stdlib.h>
#include "circbuf.h"
#include "linkedlist.h"
#include "log.h"

#define NUM_STRUCTS (20)
typedef struct test_struct {
  uint16_t test1;
  uint16_t test2;
  uint16_t test3;
} test_struct_t;

uint8_t compare(void * data1, void * data2)
{
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
  node_t * head;
  int32_t index;

  log_init();
  ll_init(&head);
  uint8_t * test;

  for (uint16_t i = 0; i < 10; i++)
  {
    test = malloc(sizeof(*test));
    *test = i;
    ll_insert(head, test, INSERT_AT_END);
  }

  test = malloc(sizeof(*test));
  *test = 4;
  ll_search(head, test, compare, &index);
  ll_remove(head, (void *)&test, 9);
  *test = 9;
  ll_search(head, test, compare, &index);
  LOG_HIGH("*p_test %d", *test);
  ll_destroy(head);
  log_destroy();
  return 0;
}
