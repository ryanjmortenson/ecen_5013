#include <stdint.h>
#include <stdlib.h>
#include "circbuf.h"
#include "log.h"

#define NUM_STRUCTS (20)
typedef struct test_struct {
  uint16_t test1;
  uint16_t test2;
  uint16_t test3;
} test_struct_t;

int main()
{
  log_init();
  circbuf_t * test;
  circbuf_init(&test, NUM_STRUCTS);

  for (uint32_t i = 0; i < NUM_STRUCTS; i++)
  {
    LOG_FATAL("MALLOCING a test_struct");
    test_struct_t * data = malloc(sizeof(*data));
    data->test1 = random();
    data->test2 = random();
    data->test3 = random();
    LOG_HIGH("%d, %d, %d", data->test1, data->test2, data->test3);
    circbuf_add_item(test, data);
  }
  for (uint32_t i = 0; i < NUM_STRUCTS; i++)
  {
    test_struct_t * data;
    circbuf_remove_item(test, (void **)&data);
    LOG_HIGH("%p, %d, %d, %d", &data, data->test1, data->test2, data->test3);
  }
  circbuf_destroy(test);
  log_destroy();
  return 0;
}
