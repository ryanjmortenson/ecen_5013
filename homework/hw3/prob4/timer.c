/** @file timer.c
*
* @brief Timer module
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mortenson");
MODULE_DESCRIPTION("Module with 500 ms timer");

/*
 * \brief list_procs_init: timer module initialization
 *
 * \return: success or error
 */
static int __init list_procs_init(void)
{
  struct task_struct *task = current;
  struct task_struct *current_task;
  struct list_head *pos;

  printk(KERN_INFO "Entered %s", __FUNCTION__);
  printk(KERN_INFO "Initializing listing processes\n");

  while (task->pid != 0)
  {
    printk("Looping over children\n");
    printk("Current task pointer: %p, pid: %d, name: %s\n", task, task->pid, task->comm);
    list_for_each(pos, &task->children)
    {
      current_task = (struct task_struct *)((char *)(list_entry(pos, struct task_struct, children)) - 0x10);
      printk(KERN_INFO "pointer %p name %s, pid %d\n", current_task, (current_task)->comm, (current_task)->pid);
    }
#if 0
    list_for_each_entry(current_task, &task->children, children)
    {
      printk(KERN_INFO "name %s, pid %d\n", current_task->comm, current_task->pid);
    }
#endif
    printk("Moving to parent\n");
    task = task->parent;
  }
  return 0;
}

/*
 * \brief list_procs_cleanup: timer module cleanup
 */
static void __exit list_procs_cleanup(void)
{
  printk(KERN_INFO "Entered %s", __FUNCTION__);
}

module_init(list_procs_init);
module_exit(list_procs_cleanup);
