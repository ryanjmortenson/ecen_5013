/** @file linkedlist.h
*
* @brief Implementation of doubly linked list
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

#define INSERT_AT_END (-1)
#define REMOVE_AT_END (-1)

// Expose node_t to linked list users
typedef struct node node_t;

// Export comparison function definition
typedef uint8_t (*COMPAREFUNC)(void * data1, void * data2);

// Export print function definition
typedef void (*PRINTFUNC)(void * data, uint32_t index);

// Enums for linked list
typedef enum ll_enum
{
  LL_ENUM_NO_ERROR,
  LL_ENUM_NULL_POINTER,
  LL_ENUM_ALLOC_FAILURE,
  LL_ENUM_FAILURE,
  LL_ENUM_INDEX_NON_EXISTENT,
  LL_ENUM_INDEX_TOO_LARGE,
  LL_DATA_NOT_FOUND
} ll_enum_t;

// Null point check macro
#define LL_CHECK_NULL(x) if (x == NULL) {return LL_ENUM_NULL_POINTER;}

/*
 * \brief ll_init: Initialize the linked list.
 *
 * \param head: pointer to head which will be malloced.
 * \return: success or error
 *
 */
ll_enum_t ll_init(node_t ** head);

/*
 * \brief ll_destroy: Destroy linked list by looping over nodes and
 *                    freeing data.
 *
 * \param head: pointer to head which will be freed.
 * \return: success or error
 *
 */
ll_enum_t ll_destroy(node_t * head);

/*
 * \brief ll_insert: Insert a node into linked list with data
 *
 * \param head: pointer to head.
 * \param data: pointer to data which will be inserted
 * \param index: index to insert (INSERT_AT_END can be used to do so)
 * \return: success or error
 *
 */
ll_enum_t ll_insert(node_t * head, void * data, int32_t index);

/*
 * \brief ll_remove: Remove a node from the list and return data
 *
 * \param head: pointer to head.
 * \param data: double pointer where data will be placed if found
 * \param index: index to insert (REMOVE_AT_END can be used to do so)
 * \return: success or error
 *
 */
ll_enum_t ll_remove(node_t * head, void ** data, int32_t index);

/*
 * \brief ll_search: Search for data using compare func
 *
 * \param head: pointer to head.
 * \param data: pointer to data for comparison in compare func
 * \param func: comparison function used to find data
 * \param index: index of node if data is found
 * \return: success or error
 *
 */
ll_enum_t ll_search(node_t * head, void * data, COMPAREFUNC func, int32_t * index);

/*
 * \brief ll_dump: Print all of linked list
 *
 * \param head: pointer to head.
 * \param func: print function used to print data
 * \return: success or error
 *
 */
ll_enum_t ll_dump(node_t * head, PRINTFUNC func);

/*
 * \brief ll_size: Gets the size of the list
 *
 * \param head: pointer to head.
 * \param size: size of list
 * \return: success or error
 *
 */
ll_enum_t ll_size(node_t * head, int32_t * size);
#endif /* _LINKED_LIST_H */
