/** @file unit_conv.c
*
* @brief Unit tests for conv.c
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <mqueue.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>

#include "project_defs.h"
#include "tmp102.h"

int temps[][3] = {
  {0x7ff, 128, 127},
  {0x640, 101, 99},
  {0x500, 81, 79},
  {0x4B0, 76, 74},
  {0x320, 51, 49},
  {0x190, 26, 24},
  {0x004, 1, 0},
  {0x000, 1, -1},
  {0xffc, 0, -1},
  {0xe70, -24, -26},
  {0xc90, -54, -56}
};

void test_temp_conv(void **state)
{
  float conversion;

  for (uint8_t i = 0; i < 11; i++)
  {
    conversion = convert_temp(temps[i][0]);
    assert_int_equal((conversion < temps[i][1] && conversion > temps[i][2]), 1);
  }
}
