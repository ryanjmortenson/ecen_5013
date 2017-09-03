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

typedef struct node node_t;
typedef uint8_t (*COMPAREFUNC)(void * data1, void * data2);

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

#define LL_CHECK_NULL(x) if (x == NULL) {return LL_ENUM_NULL_POINTER;}

ll_enum_t ll_init(node_t ** head);

ll_enum_t ll_destroy(node_t * head);

ll_enum_t ll_insert(node_t * head, void * data, int32_t index);

ll_enum_t ll_remove(node_t * head, void ** data, int32_t index);

ll_enum_t ll_search(node_t * head, void * data, COMPAREFUNC func, int32_t * index);

uint32_t ll_size(node_t * head, int32_t * index);
#endif /* _LINKED_LIST_H */
