/** @file main.c
*
* @brief Main Task functionality
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "main_task.h"
#include "project_defs.h"

int main(int argc, char *argv[])
{
  if (init_main_task(argc, argv) != SUCCESS)
  {
    return 1;
  }

  main_task();

  if (dest_main_task() != SUCCESS)
  {
    return 1;
  }
  return 0;
}
