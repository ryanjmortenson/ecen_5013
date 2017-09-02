#include <stdint.h>
#include <stdlib.h>

extern int __SYSMEM_SIZE;
int main()
{
  __SYSMEM_SIZE = 4096;
  char * test2 = (char *)malloc(2048);
  uint8_t test = 1;
  test = test;
  return 0;
}
