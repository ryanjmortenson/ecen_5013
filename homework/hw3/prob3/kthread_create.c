/** @file timer.c
*
* @brief Timer module
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/timekeeping.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mortenson");
MODULE_DESCRIPTION("Test kthread_create time");

int kthread_func(void * data)
{
  printk(KERN_INFO "Entered %s", __FUNCTION__);
  return 0;
}

/*
 * \brief kthread_profile_init: kthread profile init
 *
 * \return: success or error
 */
static int __init kthread_profile_init(void)
{
  struct timespec start, stop;
  int i;

  printk(KERN_INFO "Entered %s", __FUNCTION__);
  printk(KERN_INFO "Starting kthread_create\n");

  // Create 10
  for (i = 0; i < 10; i++)
  {
    getrawmonotonic(&start);
    kthread_create(kthread_func, NULL, "%s", "kernel_thread");
    getrawmonotonic(&stop);
    printk(KERN_INFO "Thread creation time is %lu s and %lu",
           stop.tv_sec - start.tv_sec,
           stop.tv_nsec - start.tv_nsec);
  }
  return 0;
}

/*
 * \brief kthread_profile_cleanup: kthread profile destroy
 */
static void __exit kthread_profile_cleanup(void)
{
  printk(KERN_INFO "Entered %s", __FUNCTION__);
}

module_init(kthread_profile_init);
module_exit(kthread_profile_cleanup);
