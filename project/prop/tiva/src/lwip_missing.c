/** @file lwip_missing.c
*
* @brief Functions missing from sys_arch.c in the TIVA lwip
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include "lwip/opt.h"
#include "lwip/sys.h"

/*!
* @brief Invalidate a message box
* @param mbox box to invalidate
*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  mbox->queue = SYS_MBOX_NULL;
}

/*!
* @brief Invalidate a semaphore
* @param sem semaphore to invalidate
*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem = NULL;
}

/*!
* @brief Check if semaphore is valid
* @param sem semaphore to check
* @return if semaphore is valid
*/
int sys_sem_valid(sys_sem_t *sem)
{
  if (sem != NULL)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
