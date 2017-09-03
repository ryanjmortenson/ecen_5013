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

static uint32_t count;


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
  count = 0;

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
  node_t * next = head;
  node_t * new = NULL;

  if (index == INSERT_AT_END)
  {
    index = count;
  }

  if (index > count)
  {
    return LL_ENUM_INDEX_TOO_LARGE;
  }

  if ((new = malloc(sizeof(node_t))) == NULL)
  {
    return LL_ENUM_ALLOC_FAILURE;
  }

  new->data = data;

  for (uint32_t i = 0; i < index; i++)
  {
    next = next->next;
  }

  next->next = new;
  new->prev = next;
  count++;
  return  LL_ENUM_NO_ERROR;
} // ll_insert()

ll_enum_t ll_remove(node_t * head, void ** data, int32_t index)
{
  FUNC_ENTRY;

  LL_CHECK_NULL(head);
  LL_CHECK_NULL(data);
  node_t * next = head->next;

  for (uint32_t i = 0; i < index; i++)
  {
    next = next->next;
  }
  *data = next->data;
  next->prev->next = next->next;
  if (next->next != NULL)
  {
    next->next->prev = next->prev;
  }
  free(next);
  count--;
  return  LL_ENUM_NO_ERROR;
} // ll_remove()

ll_enum_t ll_search(node_t * head, void * data, COMPAREFUNC func, int32_t * index)
{
  FUNC_ENTRY;
  node_t * next = head->next;

  for (uint32_t i = 0; i < count; i++)
  {

    if (func(data, next->data))
    {
      LOG_LOW("Found a match at %p, index %d", next, i);
      *index = i;
      return  LL_ENUM_NO_ERROR;
    }
    else
    {
      next = next->next;
    }
  }
  LOG_ERROR("Data not found");
  return LL_DATA_NOT_FOUND;
} // ll_search()

uint32_t ll_size(node_t * head, void * data, uint32_t index)
{
  FUNC_ENTRY;

  return  LL_ENUM_NO_ERROR;
} // ll_search()

ll_enum_t ll_destroy(node_t * head)
{
  node_t * next = head->next;
  for (uint32_t i = 0; i < count - 1; i++)
  {
    next = next->next;
    free(next->prev->data);
    free(next->prev);
  }
  free(head);
  return  LL_ENUM_NO_ERROR;
}
