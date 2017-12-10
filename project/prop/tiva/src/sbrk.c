/** @file sbrk.c
*
* @brief Make an sbrk to use with malloc
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <sys/stat.h>
#include <stdint.h>

uint32_t * heap_end = 0;

// This code was adapted from:
// https://balau82.wordpress.com/2010/12/16/using-newlib-in-arm-bare-metal-programs/

/*!
* @brief Adjust end of data segment
* @param[in] incr how far to adjust into heap
* @return new heap end
*/
caddr_t _sbrk(int incr) {
  extern uint32_t heap_low;
  uint32_t *prev_heap_end;

  // Set the heap end if this is the first call
  if (heap_end == 0) {
    heap_end = &heap_low;
  }

  // Set previous heap end
  prev_heap_end = heap_end;

  // Increment current heap end for next call
  heap_end += incr;

  // Return previous heap end
  return (caddr_t) prev_heap_end;
}
