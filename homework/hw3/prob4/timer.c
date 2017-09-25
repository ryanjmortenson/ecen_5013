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

void print_children(struct task_struct * task, struct task_struct * prev_task)
{
  struct list_head *pos;
  struct task_struct * child_task;
  // printk(KERN_INFO "prev_task name: %s, task name: %s\n", prev_task->comm, task->comm);
  list_for_each(pos, &task->children)
  {
    child_task = list_entry(pos, struct task_struct, sibling);
    if (prev_task->pid != task->pid)
    {
      printk(KERN_INFO "name: %s, pid: %d\n", child_task->comm, child_task->pid);
      print_children(child_task, prev_task);
    }
  }
}

/*
 * \brief list_procs_init: timer module initialization
 *
 * \return: success or error
 */
static int __init list_procs_init(void)
{
  struct task_struct *task = current->parent;
  struct task_struct *prev_task = current;

  printk(KERN_INFO "Entered %s", __FUNCTION__);
  printk(KERN_INFO "Initializing listing processes\n");

  while (task->pid != 0)
  {
    prev_task = task;
    task = task->parent;
    print_children(task, prev_task);
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
