/** @file syscall.c
*
* @brief Calls the system call function for testing
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <errno.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define SYS_BUF_SORT (333)
#define BUF_LEN (256)

int main()
{
  int32_t res = 0;
  int32_t input[BUF_LEN] = {0};
  int32_t output[BUF_LEN] = {0};

  // Generate random data
  for (int i = 0; i < BUF_LEN; i++)
  {
    input[i] = random();
    printf("%d\n", input[i]);
  }
  printf("\n");

  // Call system call to sort
  res = syscall(SYS_BUF_SORT, input, BUF_LEN, output);
  printf("Good system call result %d\n", res);
  printf("Good system call errno %s\n", strerror(errno));
  for (int i = 0; i < BUF_LEN; i++)
  {
    printf("%d\n", output[i]);
  }
  printf("\n");

  // Call system call with bad paraemters to ensure if fails correctly
  res = syscall(SYS_BUF_SORT, NULL, BUF_LEN, output);
  printf("Null input pointer result %d\n", res);
  printf("Null input pointer errno %s\n", strerror(errno));
  res = syscall(SYS_BUF_SORT, input, BUF_LEN, NULL);
  printf("Null output pointer result %d\n", res);
  printf("Null output pointer errno %s\n", strerror(errno));
  res = syscall(SYS_BUF_SORT, input, -1, output);
  printf("Bad length result %d\n", res);
  printf("Bad length errno %s\n", strerror(errno));
  return 0;
}
