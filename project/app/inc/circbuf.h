/** @file circbuf.h
*
* @brief Interface for circular buffer
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef __CIRCBUF_H__
#define __CIRCBUF_H__

#include <stdint.h>

// Circbuf typedef
typedef struct circbuf circbuf_t;

// Export print function definition
typedef void (*PRINTFUNC)(void * data, uint32_t index);

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
 * \brief circbuf_destroy: calls free on the buffer and the structure
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success or error
 *
 */
cb_enum_t circbuf_destroy(circbuf_t * buf);

/*
 * \brief circbuf_destroy_free: destroys list and frees data
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success or error
 *
 */
cb_enum_t circbuf_destroy_free(circbuf_t * buf);

/*
 * \brief circbuf_add_item: adds and item to the head pointer and increments
 *                           head
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param payload: payload to be added to circular buffer
 * \return: success or error
 *
 */
cb_enum_t circbuf_add_item(circbuf_t * buf, void * payload);

/*
 * \brief circbuf_remove_item: removes and item from tail and in payload
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param payload: memory location where removed item will be placed
 * \return: success or error
 *
 */
cb_enum_t circbuf_remove_item(circbuf_t * buf, void ** payload);

/*
 * \brief circbuf_full: checks if buffer is full
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success if full or error if not full
 *
 */
cb_enum_t circbuf_full(circbuf_t * buf);

/*
 * \brief circbuf_empty: checks if buffer is empty
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success if empty or error if not empty
 *
 */
cb_enum_t circbuf_empty(circbuf_t * buf);

/*
 * \brief circbuf_peek: gets the index item from tail
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param index: index from tail to get payload from
 * \param payload: memory location to place item
 * \return: success if empty or error if not empty
 *
 */
cb_enum_t circbuf_peek(circbuf_t * buf, uint32_t index, void ** payload);

/*
 * \brief circbuf_dump: prints contents of circbuf
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \param func: function for printing data
 * \return: success if empty or error if not empty
 *
 */
cb_enum_t circbuf_dump(circbuf_t * buf, PRINTFUNC func);

/*
 * \brief circbuf_null_buffer: nulls internal buffer
 *
 * \param buf: pointer to a pointer for the circular buffer structure
 * \return: success/fail
 *
 */
cb_enum_t circbuf_null_buffer(circbuf_t * buf);
#endif // __CIRCBUF_H__
