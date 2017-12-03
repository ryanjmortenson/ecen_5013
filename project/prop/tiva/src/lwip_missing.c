#include "lwip/opt.h"
#include "lwip/sys.h"

void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  mbox->queue = SYS_MBOX_NULL;
}

void sys_sem_set_invalid(sys_sem_t *sem)
{
    sem = NULL;
}

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
