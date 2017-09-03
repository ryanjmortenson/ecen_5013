/** @file linkedlist.c
*
* @brief Implementation of doubly linked list
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <stdint.h>
#include <stdlib.h>
#include "project_defs.h"
#include "linkedlist.h"
#include "log.h"

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

  if ((*head = malloc(sizeof(node_t))) == NULL)
  {
    return LL_ENUM_ALLOC_FAILURE;
  }

  (*head)->next = NULL;
  (*head)->prev = NULL;
  (*head)->data = NULL;

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

  while (current->next != NULL)
  {
    current = current->next;
    if (count != INSERT_AT_END && count == index)
    {
      break;
    }
    count++;
  }

  if (index != INSERT_AT_END && count != index)
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  if ((new = malloc(sizeof(node_t))) == NULL)
  {
    return LL_ENUM_ALLOC_FAILURE;
  }

  new->data = data;
  new->prev = current;
  current->next = new;
  return  LL_ENUM_NO_ERROR;
} // ll_insert()

ll_enum_t ll_remove(node_t * head, void ** data, int32_t index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);

  node_t * current = head;
  uint32_t count = 0;

  while (current->next != NULL)
  {
    current = current->next;
    if (count != REMOVE_AT_END && count == index)
    {
      break;
    }
    count++;
  }

  *data = current->data;
  current->prev->next = current->next;
  free(current);
  return  LL_ENUM_NO_ERROR;
} // ll_remove()

ll_enum_t ll_search(node_t * head, void * data, COMPAREFUNC func, int32_t * index)
{
  FUNC_ENTRY;
  node_t * current = head;
  uint32_t count = 0;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);
  LL_CHECK_NULL(func);
  LL_CHECK_NULL(index);

  while(current->next != NULL)
  {
    current = current->next;
    if (func(data, current->data))
    {
      *index = count;
      return  LL_ENUM_NO_ERROR;
    }
    count++;
  }
  return LL_DATA_NOT_FOUND;
} // ll_search()

ll_enum_t ll_size(node_t * head, int32_t * size)
{
  FUNC_ENTRY;
  node_t * current = head;
  uint32_t count = 0;

  while(current->next != NULL)
  {
    current = current->next;
    count++;
  }
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
  node_t * current = head;
  while(current->next != NULL)
  {
    current = current->next;
    free(current->prev->data);
    free(current->prev);
  }
  free(head);
  return  LL_ENUM_NO_ERROR;
} // ll_destroy()
