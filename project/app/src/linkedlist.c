/** @file linkedlist.c
*
* @brief Implementation of doubly linked list
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"
#include "log.h"

// Remove comments if you want function prints from ll_iter functions
//#define LL_ITER_DEBUG

// Linked list node structure
struct node
{
  struct node * next;
  struct node * prev;
  void * data;
};

ll_enum_t ll_init(node_t ** head)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);

  // Alloc a new head
  if ((*head = malloc(sizeof(node_t))) == NULL)
  {
    return LL_ENUM_ALLOC_FAILURE;
  }

  // Memset head
  memset(*head, 0, sizeof(**head));

  return  LL_ENUM_NO_ERROR;
} // ll_init()

ll_enum_t ll_insert(node_t * head, void * data, int32_t index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);

  node_t * current = head;
  node_t * new = NULL;
  volatile int32_t count = 0;

  // Look for index of end of list
  while (current->next != NULL)
  {
    current = current->next;
    if (count != INSERT_AT_END && count == index)
    {
      break;
    }
    count++;
  }

  // Couldn't find index
  if (index != INSERT_AT_END && count != index)
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  // Create a new node
  if ((new = malloc(sizeof(*new))) == NULL)
  {
    return LL_ENUM_ALLOC_FAILURE;
  }

  // Memset and initialize new node
  memset(new, 0, sizeof(*new));
  new->data = data;
  new->prev = current;
  new->next = current->next;
  current->next = new;
  if (new->next != NULL)
  {
    new->next->prev = new;
  }

  return  LL_ENUM_NO_ERROR;
} // ll_insert()

ll_enum_t ll_remove(node_t * head, void ** data, int32_t index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);

  node_t * current = head;
  int32_t count = -1;

  // Look for index
  while (current->next != NULL)
  {
    if (count != REMOVE_AT_END && count == index)
    {
      break;
    }
    current = current->next;
    count++;
  }

  // Couldn't find index
  if (index != REMOVE_AT_END && count != index)
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  // Node was found, remove and set data pointer
  *data = current->data;
  current->prev->next = current->next;
  if (current->next != NULL)
  {
    current->next->prev = current->prev;
  }

  // Free the current node
  free(current);

  return  LL_ENUM_NO_ERROR;
} // ll_remove()

ll_enum_t ll_search(node_t * head, void * data, COMPAREFUNC func, int32_t * index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);
  LL_CHECK_NULL(func);
  LL_CHECK_NULL(index);

  node_t * current = head;
  uint32_t count = 0;

  // Look through node using compare function to find data
  while(current->next != NULL)
  {
    current = current->next;
    if (func(data, current->data))
    {
      // Data was found return the rest
      *index = count;
      return  LL_ENUM_NO_ERROR;
    }
    count++;
  }
  return LL_DATA_NOT_FOUND;
} // ll_search()

ll_enum_t ll_peek(node_t * head, void ** data, int32_t index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);

  node_t * current = head;
  uint32_t count = 0;

  // Look for index
  while (current->next != NULL)
  {
    current = current->next;
    if (count == index)
    {
      break;
    }
    count++;
  }

  // Couldn't find index
  if (count != index)
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  // Node was found, remove and set data pointer
  *data = current->data;

  return LL_ENUM_NO_ERROR;
} // ll_peek()

ll_enum_t ll_size(node_t * head, int32_t * size)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(size);

  node_t * current = head;
  uint32_t count = 0;

  // Loop until the end is found
  while(current->next != NULL)
  {
    current = current->next;
    count++;
  }

  // Set size
  *size = count;
  return  LL_ENUM_NO_ERROR;
} // ll_size()

ll_enum_t ll_dump(node_t * head, PRINTFUNC func)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(func);

  node_t * current = head;
  uint32_t count = 0;

  // Loop over list calling the print function
  while(current->next != NULL)
  {
    current = current->next;
    func(current->data, count);
    count++;
  }
  return  LL_ENUM_NO_ERROR;
} // ll_dump()

ll_enum_t ll_destroy(node_t * head)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);

  node_t * current = head;
  uint32_t count = 0;

  // Loop over list freeing data and nodes
  while(current->next != NULL)
  {
    current = current->next;
    free(current->data);
    free(current->prev);
    count++;
  }

  // Free the last node
  free(current);
  return  LL_ENUM_NO_ERROR;
} // ll_destroy()

ll_enum_t ll_iter(node_t * head, node_t ** iter)
{
#ifdef LL_ITER_DEBUG
  FUNC_ENTRY;
#endif

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(iter);

  *iter = head;

  return  LL_ENUM_NO_ERROR;
} // ll_iterator()

ll_enum_t ll_iter_next(node_t ** iter, void ** data)
{
#ifdef LL_ITER_DEBUG
  FUNC_ENTRY;
#endif

  LL_CHECK_NULL(data);
  LL_CHECK_NULL(iter);


  if ((*iter)->next != NULL)
  {
    *iter = (*iter)->next;
    *data = (*iter)->data;
  }
  else
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  return  LL_ENUM_NO_ERROR;
} // ll_iterator()
