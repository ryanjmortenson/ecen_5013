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
