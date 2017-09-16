/** @file timer.c
*
* @brief Timer module
* @author Ryan Mortenson
* @tools GCC 5.4.0, vim 7.4, make 4.1, Ubuntu 16.04
*
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>

#define TIMER_MS (500)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mortenson");
MODULE_DESCRIPTION("Module with 500 ms timer");

static struct timer_list time;

/*
 * \brief timer_cb: Called when 500 ms timer pops
 *
 * \param data: some data
 *
 */
void timer_cb(unsigned long data)
{
  static int i;
  int ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  i++;
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(TIMER_MS));

  if (ret != 1)
  {
    printk(KERN_INFO "Error in modifying timer");
  }

  printk(KERN_INFO "Called %d times", i);
}

/*
 * \brief timer_init: timer module initialization
 *
 * \return: success or error
 */
static int __init timer_init(void)
{
  int ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);
  printk(KERN_INFO "Initializing timer");

  setup_timer(&time, timer_cb, 0);
  ret = mod_timer(&time, jiffies + msecs_to_jiffies(TIMER_MS));
  if (ret != 0)
  {
    printk("Error in modifying timer");
  }

  return 0;
}

/*
 * \brief timer_cleanup: timer module cleanup
 */
static void __exit timer_cleanup(void)
{
  int ret = 0;
  printk(KERN_INFO "Entered %s", __FUNCTION__);

  ret = del_timer(&time);
  if (ret != 0)
  {
    printk("Error in cleaning up timer");
  }
}

module_init(timer_init);
module_exit(timer_cleanup);
