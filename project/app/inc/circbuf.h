#ifndef __CIRCBUF_H__
#define __CIRCBUF_H__

#include <stdint.h>
#include <stddef.h>
#include "project_defs.h"

// Check for null pointer
#define CB_CHECK_NULL(x) if (x == NULL) {return CB_ENUM_NULL_POINTER;}

// Status enum for circular buffer functions
typedef enum cb_enum
{
  CB_ENUM_NO_ERROR,
  CB_ENUM_FULL,
  CB_ENUM_EMPTY,
  CB_ENUM_NULL_POINTER,
  CB_ENUM_NO_LENGTH,
  CB_ENUM_ALLOC_FAILURE,
  CB_ENUM_FAILURE,
  CB_ENUM_BAD_INDEX
} cb_enum_t;

// Circular buffer structure
typedef struct circbuf
{
  uint8_t * buffer;
  volatile uint8_t * head;
  volatile uint8_t * tail;
  volatile uint16_t count;
  uint16_t length;
} circbuf_t;

/*
 * \brief circbuf_init: Initialize circular buffer with a length this will
 *                       call malloc to put the buffer and the structure
 *                       on the heap
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param length: length of the circular buffer
 * \return: success or error
 *
 */
cb_enum_t circbuf_init(circbuf_t ** buf, uint16_t length);

/*
 * \brief circbuf_init: Initialize circular buffer with a length this will
 *                       call malloc to put the buffer and the structure
 *                       on the heap
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param length: length of the circular buffer
 * \return: success or error
 *
 */
cb_enum_t circbuf_init_dma(circbuf_t ** buf, uint16_t length, uint8_t * addr);

/*
 * \brief circbuf_destroy: calls free on the buffer and the structure
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success or error
 *
 */
cb_enum_t circbuf_destroy(circbuf_t * buf);

/*
 * \brief circbuf_add_item: adds and item to the head pointer and increments
 *                           head
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param payload: payload to be added to circular buffer
 * \return: success or error
 *
 */
cb_enum_t circbuf_add_item(circbuf_t * buf, uint8_t payload);

/*
 * \brief circbuf_remove_item: removes and item from tail and in payload
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param payload: memory location where removed item will be placed
 * \return: success or error
 *
 */
cb_enum_t circbuf_remove_item(circbuf_t * buf, uint8_t * payload);

/*
 * \brief circbuf_full: checks if buffer is full
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success if full or error if not full
 *
 */
__attribute__((always_inline))static inline cb_enum_t circbuf_full(circbuf_t * buf)
{
  // Check null pointer
  CB_CHECK_NULL(buf);

  // Buffer is full return success
  if (buf->length == buf->count)
  {
    return CB_ENUM_FULL;
  }

  // Buffer is not full return failure
  return CB_ENUM_FAILURE;
} // circbuf_full()

/*
 * \brief circbuf_empty: checks if buffer is empty
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success if empty or error if not empty
 *
 */
__attribute__((always_inline))static inline cb_enum_t circbuf_empty(circbuf_t * buf)
{
  // Check null pointer
  CB_CHECK_NULL(buf);

  // Buffer is full return success
  if (buf->count == 0)
  {
    return CB_ENUM_EMPTY;
  }

  // Buffer is not full return failure
  return CB_ENUM_FAILURE;
} // circbuf_empty()

/*
 * \brief circbuf_peek: gets the index item from tail
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param index: index from tail to get payload from
 * \param payload: memory location to place item
 * \return: success if empty or error if not empty
 *
 */
cb_enum_t circbuf_peek(circbuf_t * buf, uint32_t index, uint8_t * payload);

/*
 * \brief circbuf_null_buffer: nulls internal buffer
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success/fail
 *
 */
cb_enum_t circbuf_null_buffer(circbuf_t * buf);
#endif // __CIRCBUF_H__
